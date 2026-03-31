import pandas as pd
import numpy as np
from tabpfn import TabPFNRegressor
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import KFold
from sklearn.metrics import mean_squared_error

# -----------------------------
# 0. Reproducibility
# -----------------------------
np.random.seed(42)

# -----------------------------
# 1. Load data
# -----------------------------
train_df = pd.read_csv("burn_calories_train.csv")
test_df = pd.read_csv("burn_calories_test.csv")

# -----------------------------
# 1.1 Feature engineering (safe)
# -----------------------------
def add_features(df: pd.DataFrame) -> pd.DataFrame:
    d = df.copy()
    # Guard divisions
    duration_safe = d["Duration"].replace(0, 1)
    height_safe = d["Height"].replace(0, np.nan)
    # Core derived features
    d["BMI"] = d["Weight"] / ((d["Height"] / 100.0) ** 2)
    d["HR_per_min"] = d["Heart_Rate"] / duration_safe
    d["Temp_per_min"] = d["Body_Temp"] / duration_safe
    d["Age_Weight"] = d["Age"] * d["Weight"]
    d["Height_Age"] = d["Height"] * d["Age"]
    # Extra interactions
    d["Weight_Height"] = d["Weight"] / height_safe
    d["HRxTemp"] = d["Heart_Rate"] * d["Body_Temp"]
    # Clean inf/nan from divisions
    d = d.replace([np.inf, -np.inf], np.nan).fillna(0)
    return d

train_df = add_features(train_df)
test_df = add_features(test_df)

# -----------------------------
# 2. Encode Gender safely
# -----------------------------
def encode_gender(x):
    if str(x).lower().startswith("m"):
        return 0
    if str(x).lower().startswith("f"):
        return 1
    return 0  # fallback

train_df["Gender"] = train_df["Gender"].apply(encode_gender)
test_df["Gender"] = test_df["Gender"].apply(encode_gender)

# -----------------------------
# 3. Define features
# -----------------------------
FEATURES = [
    "Gender",
    "Age",
    "Height",
    "Weight",
    "Duration",
    "Heart_Rate",
    "Body_Temp",
    # added features
    "BMI", "HR_per_min", "Temp_per_min", "Age_Weight", "Height_Age",
    "Weight_Height", "HRxTemp"
]

X_train = train_df[FEATURES].copy()
y_train = train_df["Calories"]

X_test = test_df[FEATURES].copy()

# -----------------------------
# 4. Scale numeric features
# -----------------------------
num_features = [c for c in FEATURES if c != "Gender"]

scaler = StandardScaler()
X_train[num_features] = scaler.fit_transform(X_train[num_features])
X_test[num_features] = scaler.transform(X_test[num_features])

# -----------------------------
# 5. KFold CV + averaged predictions
# -----------------------------
kf = KFold(n_splits=5, shuffle=True, random_state=42)
test_pred_accum = np.zeros(len(X_test))
rmse_scores = []
for fold, (tr_idx, val_idx) in enumerate(kf.split(X_train)):
    X_tr, X_val = X_train.iloc[tr_idx], X_train.iloc[val_idx]
    y_tr, y_val = y_train.iloc[tr_idx], y_train.iloc[val_idx]
    model = TabPFNRegressor(device="cpu", ignore_pretraining_limits=True, n_estimators=8)
    model.fit(X_tr.values, y_tr.values)
    val_pred = model.predict(X_val.values)
    # manual RMSE for broad sklearn compat
    fold_rmse = mean_squared_error(y_val, val_pred) ** 0.5
    rmse_scores.append(fold_rmse)
    print(f"Fold {fold+1} RMSE: {fold_rmse:.5f}")
    test_pred_accum += model.predict(X_test.values) / kf.n_splits
print(f"CV RMSE: {np.mean(rmse_scores):.5f} +/- {np.std(rmse_scores):.5f}")
y_pred = test_pred_accum

# -----------------------------
# 7. Create submission
# -----------------------------
submission = pd.DataFrame({
    "User_ID": test_df["User_ID"],
    "Calories": y_pred
})

submission.to_csv("submission.csv", index=False)
print("submission.csv ready ✅")