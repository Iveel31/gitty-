"""BUUZ instance segmentation (Detectron2) - Kaggle-ready training + submission pipeline.

Run in Kaggle Notebook (GPU):
1) Install deps in a cell:
   !pip install -q 'git+https://github.com/facebookresearch/detectron2.git'
   !pip install -q pycocotools
2) Put your dataset under `/kaggle/input/<dataset_name>/` with structure:
   train/annotations.json, train/images/
   val/annotations.json, val/images/
   test/images/
3) Execute this script.
"""

from __future__ import annotations

import gc
import importlib
import json
import os
import shutil
import subprocess
import sys
from copy import deepcopy
from pathlib import Path

import cv2
import numpy as np
import pandas as pd
import torch
from tqdm import tqdm


def ensure_module(module_name: str, install_cmd: list[str]) -> None:
    try:
        importlib.import_module(module_name)
    except ModuleNotFoundError:
        print(f"Installing missing dependency: {module_name}")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "-q", *install_cmd])


ensure_module("pycocotools", ["pycocotools"])
ensure_module("detectron2", ["git+https://github.com/facebookresearch/detectron2.git"])

from pycocotools import mask as mask_utils

import detectron2
from detectron2 import model_zoo
from detectron2.config import get_cfg
from detectron2.data import DatasetCatalog
from detectron2.data import DatasetMapper
from detectron2.data import MetadataCatalog
from detectron2.data import build_detection_test_loader
from detectron2.data import transforms as T
from detectron2.data.datasets import register_coco_instances
from detectron2.engine import DefaultPredictor
from detectron2.engine import DefaultTrainer
from detectron2.evaluation import COCOEvaluator
from detectron2.evaluation import inference_on_dataset


def find_data_dir() -> Path:
    explicit = os.getenv("DATA_DIR")
    if explicit:
        path = Path(explicit)
        if path.exists():
            return path

    direct_candidates = [
        Path("/kaggle/input/1-3"),
        Path("/kaggle/input/buuz"),
    ]
    for candidate in direct_candidates:
        if (candidate / "train" / "annotations.json").exists() and (candidate / "test" / "images").exists():
            return candidate

    root = Path("/kaggle/input")
    if root.exists():
        for candidate in root.iterdir():
            if not candidate.is_dir():
                continue
            if (candidate / "train" / "annotations.json").exists() and (candidate / "test" / "images").exists():
                return candidate

    raise FileNotFoundError(
        "Dataset folder not found. Set DATA_DIR env var or ensure /kaggle/input/<dataset>/ has train/val/test structure."
    )


def safe_copy_tree(src_dir: Path, dst_dir: Path) -> None:
    dst_dir.mkdir(parents=True, exist_ok=True)
    for item in src_dir.glob("*"):
        if item.is_file():
            shutil.copy2(item, dst_dir / item.name)


def decode_area(segmentation, height: int, width: int) -> float:
    if isinstance(segmentation, dict) and "counts" in segmentation and "size" in segmentation:
        rle = segmentation
        if not isinstance(rle["counts"], str):
            rle = mask_utils.frPyObjects(segmentation, segmentation["size"][0], segmentation["size"][1])
        return float(mask_utils.area(rle))

    if isinstance(segmentation, list):
        rles = mask_utils.frPyObjects(segmentation, height, width)
        rle = mask_utils.merge(rles)
        return float(mask_utils.area(rle))

    return 0.0


def fix_coco_annotations(src_json: Path, dst_json: Path) -> None:
    with src_json.open("r", encoding="utf-8") as f:
        coco = json.load(f)

    coco.setdefault("info", {"description": "buuz dataset", "version": "1.0"})
    coco.setdefault("licenses", [])
    coco.setdefault("images", [])
    coco.setdefault("annotations", [])
    coco["categories"] = [{"id": 1, "name": "buuz", "supercategory": "food"}]

    image_hw = {img["id"]: (img.get("height", 0), img.get("width", 0)) for img in coco["images"]}

    valid_anns = []
    for ann in coco["annotations"]:
        ann = deepcopy(ann)
        ann.setdefault("iscrowd", 0)
        ann["category_id"] = 1

        if ann.get("area", 0) <= 0:
            h, w = image_hw.get(ann.get("image_id"), (0, 0))
            ann["area"] = decode_area(ann.get("segmentation"), h, w)

        if ann.get("area", 0) > 0:
            valid_anns.append(ann)

    coco["annotations"] = valid_anns
    dst_json.parent.mkdir(parents=True, exist_ok=True)
    with dst_json.open("w", encoding="utf-8") as f:
        json.dump(coco, f)

    print(f"Fixed {src_json.name}: {len(coco['images'])} images, {len(valid_anns)} annotations")


def merge_coco(train_json: Path, val_json: Path, out_json: Path, trainval_img_dir: Path, val_img_dir: Path) -> None:
    with train_json.open("r", encoding="utf-8") as f:
        train_coco = json.load(f)
    with val_json.open("r", encoding="utf-8") as f:
        val_coco = json.load(f)

    safe_copy_tree(val_img_dir, trainval_img_dir)

    max_img_id = max((img["id"] for img in train_coco["images"]), default=0)
    max_ann_id = max((ann["id"] for ann in train_coco["annotations"]), default=0)

    image_id_map = {}
    for img in val_coco["images"]:
        new_img = deepcopy(img)
        new_img["id"] = img["id"] + max_img_id
        image_id_map[img["id"]] = new_img["id"]
        train_coco["images"].append(new_img)

    for ann in val_coco["annotations"]:
        if ann["image_id"] not in image_id_map:
            continue
        new_ann = deepcopy(ann)
        new_ann["id"] = ann["id"] + max_ann_id
        new_ann["image_id"] = image_id_map[ann["image_id"]]
        train_coco["annotations"].append(new_ann)

    train_coco.setdefault("info", {"description": "buuz dataset", "version": "1.0"})
    train_coco.setdefault("licenses", [])

    out_json.parent.mkdir(parents=True, exist_ok=True)
    with out_json.open("w", encoding="utf-8") as f:
        json.dump(train_coco, f)

    print(f"Merged train+val: {len(train_coco['images'])} images, {len(train_coco['annotations'])} annotations")


def prepare_dataset(data_dir: Path, work_dir: Path) -> None:
    for split in ["train", "val", "test"]:
        safe_copy_tree(data_dir / split / "images", work_dir / split / "images")

    fix_coco_annotations(data_dir / "train" / "annotations.json", work_dir / "train" / "annotations.json")
    fix_coco_annotations(data_dir / "val" / "annotations.json", work_dir / "val" / "annotations.json")

    safe_copy_tree(work_dir / "train" / "images", work_dir / "trainval" / "images")
    merge_coco(
        work_dir / "train" / "annotations.json",
        work_dir / "val" / "annotations.json",
        work_dir / "trainval" / "annotations.json",
        work_dir / "trainval" / "images",
        work_dir / "val" / "images",
    )


def register_datasets(work_dir: Path) -> None:
    datasets = {
        "buuz_train": (work_dir / "train" / "annotations.json", work_dir / "train" / "images"),
        "buuz_val": (work_dir / "val" / "annotations.json", work_dir / "val" / "images"),
        "buuz_trainval": (work_dir / "trainval" / "annotations.json", work_dir / "trainval" / "images"),
    }

    for name, (ann_path, img_dir) in datasets.items():
        if name in DatasetCatalog.list():
            DatasetCatalog.remove(name)
        if name in MetadataCatalog.list():
            MetadataCatalog.remove(name)
        register_coco_instances(name, {}, str(ann_path), str(img_dir))

    train_dicts = DatasetCatalog.get("buuz_train")
    val_dicts = DatasetCatalog.get("buuz_val")
    train_anns = sum(len(item["annotations"]) for item in train_dicts)
    val_anns = sum(len(item["annotations"]) for item in val_dicts)

    print(f"Train: {len(train_dicts)} images / {train_anns} annotations")
    print(f"Val:   {len(val_dicts)} images / {val_anns} annotations")


class AugTrainer(DefaultTrainer):
    @classmethod
    def build_train_loader(cls, cfg):
        from detectron2.data import build_detection_train_loader

        augs = [
            T.ResizeShortestEdge(short_edge_length=(640, 672, 704, 736, 768, 800), max_size=1333, sample_style="choice"),
            T.RandomFlip(prob=0.5, horizontal=True),
            T.RandomFlip(prob=0.1, horizontal=False, vertical=True),
            T.RandomBrightness(0.85, 1.15),
            T.RandomContrast(0.85, 1.15),
            T.RandomSaturation(0.85, 1.15),
            T.RandomRotation(angle=[-12, 12]),
        ]
        mapper = DatasetMapper(cfg, is_train=True, augmentations=augs)
        return build_detection_train_loader(cfg, mapper=mapper)

    @classmethod
    def build_evaluator(cls, cfg, dataset_name, output_folder=None):
        if output_folder is None:
            output_folder = os.path.join(cfg.OUTPUT_DIR, "inference")
        return COCOEvaluator(dataset_name, output_dir=output_folder)


def build_cfg(output_dir: Path, train_name: str, test_name: str, base_lr: float, max_iter: int, steps: tuple[int, int]):
    cfg = get_cfg()
    cfg.merge_from_file(model_zoo.get_config_file("COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml"))
    cfg.MODEL.WEIGHTS = model_zoo.get_checkpoint_url("COCO-InstanceSegmentation/mask_rcnn_R_101_FPN_3x.yaml")

    cfg.DATASETS.TRAIN = (train_name,)
    cfg.DATASETS.TEST = (test_name,)
    cfg.DATALOADER.NUM_WORKERS = 2

    cfg.MODEL.ROI_HEADS.NUM_CLASSES = 1
    cfg.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.05
    cfg.MODEL.ROI_HEADS.BATCH_SIZE_PER_IMAGE = 512

    cfg.SOLVER.IMS_PER_BATCH = 2
    cfg.SOLVER.BASE_LR = base_lr
    cfg.SOLVER.MAX_ITER = max_iter
    cfg.SOLVER.WARMUP_ITERS = 200
    cfg.SOLVER.WARMUP_METHOD = "linear"
    cfg.SOLVER.STEPS = steps
    cfg.SOLVER.GAMMA = 0.5
    cfg.SOLVER.CHECKPOINT_PERIOD = 500
    cfg.SOLVER.WEIGHT_DECAY = 0.0001

    cfg.INPUT.MIN_SIZE_TRAIN = (640, 672, 704, 736, 768, 800)
    cfg.INPUT.MAX_SIZE_TRAIN = 1333
    cfg.INPUT.MIN_SIZE_TEST = 800
    cfg.INPUT.MAX_SIZE_TEST = 1333
    cfg.INPUT.MASK_FORMAT = "bitmask"

    cfg.TEST.EVAL_PERIOD = 500
    cfg.TEST.DETECTIONS_PER_IMAGE = 200

    cfg.OUTPUT_DIR = str(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    return cfg


def train_and_eval(cfg, eval_dataset: str):
    trainer = AugTrainer(cfg)
    trainer.resume_or_load(resume=False)
    trainer.train()

    evaluator = COCOEvaluator(eval_dataset, output_dir=cfg.OUTPUT_DIR)
    val_loader = build_detection_test_loader(cfg, eval_dataset)
    results = inference_on_dataset(trainer.model, val_loader, evaluator)
    return trainer, results


def mask_to_rle_submission(binary_mask: np.ndarray) -> str:
    if binary_mask is None:
        return ""
    mask = (binary_mask > 0).astype(np.uint8)
    if mask.sum() == 0:
        return ""

    pixels = mask.T.flatten()
    pixels = np.concatenate([[0], pixels, [0]])
    runs = np.where(pixels[1:] != pixels[:-1])[0] + 1
    runs[1::2] -= runs[::2]
    return " ".join(str(x) for x in runs)


def collect_test_images(test_dir: Path) -> list[Path]:
    exts = ["*.jpg", "*.jpeg", "*.png", "*.bmp"]
    paths = []
    for ext in exts:
        paths.extend(test_dir.glob(ext))
    return sorted(set(paths))


def make_submission(cfg, test_dir: Path, out_csv: Path) -> pd.DataFrame:
    predictor = DefaultPredictor(cfg)
    test_images = collect_test_images(test_dir)
    print(f"Test images: {len(test_images)}")

    rows = []
    row_id = 0

    for img_path in tqdm(test_images, desc="Predicting"):
        image = cv2.imread(str(img_path))
        if image is None:
            continue

        outputs = predictor(image)
        instances = outputs["instances"].to("cpu")
        if len(instances) == 0:
            continue

        masks = instances.pred_masks.numpy()
        for mask in masks:
            rle = mask_to_rle_submission(mask)
            if not rle:
                continue
            rows.append({"id": row_id, "ImageId": img_path.name, "EncodedPixels": rle})
            row_id += 1

    submission = pd.DataFrame(rows, columns=["id", "ImageId", "EncodedPixels"])
    if len(submission) > 0:
        submission["id"] = submission["id"].astype(int)

    submission.to_csv(out_csv, index=False)
    print(f"Saved submission: {out_csv}")
    print(f"Rows: {len(submission)}")
    print(f"Unique images: {submission['ImageId'].nunique() if len(submission) else 0}")
    print(f"Nulls:\n{submission.isna().sum() if len(submission) else 'empty submission'}")
    return submission


def main() -> None:
    data_dir = find_data_dir()
    work_dir = Path("/kaggle/working")

    print(f"Detectron2 version: {detectron2.__version__}")
    print(f"PyTorch: {torch.__version__}")
    print(f"CUDA available: {torch.cuda.is_available()}")
    if torch.cuda.is_available():
        print(f"GPU: {torch.cuda.get_device_name(0)}")
    print(f"DATA_DIR: {data_dir}")

    prepare_dataset(data_dir, work_dir)
    register_datasets(work_dir)

    cfg1 = build_cfg(
        output_dir=work_dir / "output_stage1",
        train_name="buuz_train",
        test_name="buuz_val",
        base_lr=5e-4,
        max_iter=3000,
        steps=(1500, 2500),
    )
    trainer1, results1 = train_and_eval(cfg1, "buuz_val")
    print("Stage 1 Results:", results1)

    cfg2 = build_cfg(
        output_dir=work_dir / "output_stage2",
        train_name="buuz_trainval",
        test_name="buuz_val",
        base_lr=1e-4,
        max_iter=2000,
        steps=(1000, 1500),
    )
    cfg2.MODEL.WEIGHTS = str(Path(cfg1.OUTPUT_DIR) / "model_final.pth")
    cfg2.SOLVER.WARMUP_ITERS = 100

    trainer2, results2 = train_and_eval(cfg2, "buuz_val")
    print("Stage 2 Results:", results2)

    del trainer1, trainer2
    gc.collect()
    if torch.cuda.is_available():
        torch.cuda.empty_cache()

    cfg_pred = cfg2.clone()
    cfg_pred.MODEL.WEIGHTS = str(Path(cfg2.OUTPUT_DIR) / "model_final.pth")
    cfg_pred.MODEL.ROI_HEADS.SCORE_THRESH_TEST = 0.05
    cfg_pred.TEST.DETECTIONS_PER_IMAGE = 200

    submission = make_submission(cfg_pred, work_dir / "test" / "images", work_dir / "submission.csv")
    if len(submission) > 0:
        sample_rle = submission.iloc[0]["EncodedPixels"]
        numbers = list(map(int, sample_rle.split()))
        print(f"RLE pair count: {len(numbers) // 2}")
        print(f"RLE sample: {numbers[:20]}")


if __name__ == "__main__":
    main()
