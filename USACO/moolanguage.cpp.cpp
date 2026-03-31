def main():
    import sys
    input = sys.stdin.read
    data = input().split('\n')
    
    idx = 0
    T = int(data[idx].strip())
    idx += 1
    
    for _ in range(T):
        N, C, P = map(int, data[idx].strip().split())
        idx += 1
        
        nouns = []
        transitive_verbs = []
        intransitive_verbs = []
        conjunctions = []
        
        for _ in range(N):
            word, word_type = data[idx].strip().split()
            if word_type == 'noun':
                nouns.append(word)
            elif word_type == 'transitive-verb':
                transitive_verbs.append(word)
            elif word_type == 'intransitive-verb':
                intransitive_verbs.append(word)
            elif word_type == 'conjunction':
                conjunctions.append(word)
            idx += 1
        
        max_words = 0
        sentences = []
        
        # Try to maximize the number of words used
        # First, use all intransitive verbs
        while intransitive_verbs and nouns:
            noun = nouns.pop()
            verb = intransitive_verbs.pop()
            sentences.append(f"{noun} {verb}.")
            max_words += 2
            P -= 1
        
        # Then, use transitive verbs with as many nouns as possible
        while transitive_verbs and nouns:
            noun1 = nouns.pop()
            verb = transitive_verbs.pop()
            sentence = f"{noun1} {verb}"
            max_words += 2
            noun_count = 0
            while nouns and C > 0:
                noun = nouns.pop()
                sentence += f", {noun}"
                max_words += 1
                noun_count += 1
                C -= 1
            sentence += "."
            sentences.append(sentence)
            P -= 1
        
        # Finally, join sentences with conjunctions
        while conjunctions and len(sentences) > 1:
            conj = conjunctions.pop()
            sent1 = sentences.pop()
            sent2 = sentences.pop()
            combined = f"{sent1} {conj} {sent2}"
            sentences.append(combined)
            max_words += 1
        
        # Ensure we don't exceed the number of periods
        if P < 0:
            max_words -= abs(P) * 2
            sentences = sentences[:len(sentences) + P]
        
        print(max_words)
        if sentences:
            print(' '.join(sentences))
        else:
            print()

if __name__ == "__main__":
    main()