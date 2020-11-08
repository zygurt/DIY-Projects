function [ success, return_letters ] = find_word(data, words, letters )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here


%Sort list of words by length and then by fewest unsolved letters
fprintf('\tSorting\n');
words = sortrows(words,[5,2,3],{'ascend', 'descend','descend'});

%Check that the first word hasn't been solved
if words.solved(1) == 1
    success = 1;
    return_letters = letters;
    return;
    
end

%Find all possible words that match first word in list
%Create the regex pattern
current_word = cell2mat(words(1,1).Var1(1));
pat = letters(current_word);
replacements = 0;
for n=2:length(current_word)
    for k=1:(n-1)
        if current_word(n)==current_word(k) && letters(current_word(n))=='.'
            pat = strcat(pat(1:(n-1+replacements)),sprintf('\\%d',k),pat((n+1+replacements):end));
            replacements = replacements+1;
            break;
        end
    end
end


pat = regexprep(pat,'\.','(.)');
for idx='a':'z'
    pat = regexprep(pat,idx,['(' idx ')']);
end


% fprintf('%s\n',pat);

%            Keyword   Result
%     ---------------  --------------------------------
%            'start'   Row vector of starting indices of each match
%              'end'   Row vector of ending indices of each match
%     'tokenExtents'   Cell array of extents of tokens in each match
%            'match'   Cell array of the text of each match
%           'tokens'   Cell array of the text of each token in each match
%            'names'   Structure array of each named token in each match
%            'split'   Cell array of the text delimited by each match

r = regexp(data{1,1},{pat},'start');
word_idx = [];
for n=1:length(r)
    if ~isempty(r{n}) % == 1 %iscellstr(r(n))
        if length(data{1,1}{n,1})==words.lengths(1);
            word_idx = [word_idx n];
            fprintf('%s\n',data{1,1}{n,1});
        end
    end
end

if isempty(word_idx)
    success = 0;
    return_letters = letters;
    return;
else
    for w = 1:length(word_idx)
        process_next = 1;
        test_word = data{1,1}{word_idx(w),1};
        
        %Set remaining letters based on the word
        temp_letters = letters;
        for n=1:length(current_word)
            if sum(letters(letters==test_word(n)))>0 && letters(current_word(n)) ~= test_word(n)
                return_letters = temp_letters;
                success = 0;
                process_next = 0;
                break;
            else
                letters(current_word(n)) = test_word(n);
            end
        end
        
        words.solved(1) = 1;
        
        %Self recursive call
        if process_next == 1
            [success, return_letters] = find_word(data, words, letters );
        end
        if success
            return;
        else
            words.solved(1) = 0;
            letters = temp_letters;
        end
        
        
    end
    
end

end

