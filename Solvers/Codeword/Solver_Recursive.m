%Number Solver
%MATLAB 2016a
% Tim Roberts November 2020
close all
clear all
clc

fid = fopen('words_alpha.txt');
data = textscan(fid,'%s');
fclose(fid);

%C-HR
% % Horizontal
% word{1} = [20 9 8 12];
% word{2} = [11 7 7 26];
% word{3} = [10 14 26];
% word{4} = [20 7 18];
% word{5} = [12 23 6 9 13 2 24 17 15];
% word{6} = [13 7 16 11];
% word{7} = [26 24 9 18 12];
% word{8} = [9 1 24 19];
% word{9} = [7 6 17];
% word{10} = [11 7 8];
% word{11} = [4 7 22 24];
% word{12} = [3 18 26 12 18];
% word{13} = [14 21 5 12];
% word{14} = [6 17 25 22 14 6 14 21 3];
% word{15} = [4 17 8];
% word{16} = [7 15 15];
% word{17} = [4 6 7 3];
% word{18} = [26 9 8 12];
% % Vertical
% word{19} = [26 14 9 21 7];
% word{20} = [7 16 11 17 6];
% word{21} = [12 23 11 7 24 9 6];
% word{22} = [9 6 23];
% word{23} = [17 9 6];
% word{34} = [26 17 3];
% word{24} = [12 11 9 24 24];
% word{25} = [18 7 22 21 3];
% word{26} = [11 7 2 2 18];
% word{27} = [12 18 6 22 26];
% word{28} = [12 11 18];
% word{29} = [7 8 17];
% word{30} = [21 7 8];
% word{31} = [15 6 9 8 14 21 3];
% word{32} = [19 7 8 17 24];
% word{33} = [5 14 21 15 12];
% 
% answer_word = [2 6 12 5 17 16];
% 
% known_pos = [13 14 21 22 24];
% known_letters = ['m' 'i' 'n' 'u' 'l'];
% letters = ['.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', 'm', ...
%            'i', '.', '.', '.', '.', '.', '.', 'n', 'u', '.', 'l', '.', '.'];

%$Known Test Case
%Horizontal
word{1} = [10 13 19 26 14 14 11];
word{2} = [22 26 6 16 26 20 26];
word{3} = [6 19 4 4 20 10 6 6 7 2 9];
word{4} = [15 5 26 2];
word{5} = [9 26 17 10];
word{6} = [4 7 10 16 7 2 9];
word{7} = [12 1 10 2];
word{8} = [6 7 17 10];
word{9} = [8 7 6 16 12 25 10 20 7 2 9];
word{10} = [6 19 24 23 10 16 15];
word{11} = [4 26 11 22 10 2 15];
%Vertical
word{12} = [10 14 10 16 15 20 12 2 7 16 6];
word{13} = [6 10 26 21 10 10 8];
word{14} = [14 7 4];
word{15} = [6 5 10];
word{16} = [4 26 16 7 18 7 16];
word{17} = [8 10 15 10 16 15 7 25 10];
word{18} = [6 14 10 2 8 10 20];
word{19} = [6 3 7];
word{20} = [7 16 11];
word{21} = [9 20 26 17 7 2 9];
word{22} = [26 16 5 7 10 25 10 22 10 2 15];

answer_word = [22 12 2 3 10 11];

alphabet = 'a':'z';
known_pos = [5 7 8 15 19];
known_letters = ['h' 'i' 'd' 't' 'u'];
letters = ['.', '.', '.', '.', 'h', '.', 'i', 'd', '.', '.', '.', '.', '.', ...
           '.', 't', '.', '.', '.', 'u', '.', '.', '.', '.', '.', '.', '.'];
unknown_pos = [];
for n = 1:26
    if sum(n==known_pos)==0
        unknown_pos = [unknown_pos n];
    end
end
pos = 1;
remaining_letters = [];
for n = 1:length(alphabet)
    if sum(alphabet(n) == known_letters)<1
        remaining_letters(pos) = char(alphabet(n));
        pos = pos+1;
    end
end
remaining_letters = char(remaining_letters);

%Create table of words
words = cell2table(word');
lengths = zeros(1,height(words));
known_letters = zeros(1,height(words));
rem_letters = zeros(1,height(words));
for n=1:height(words)
    lengths(n) = length(cell2mat(words(n,1).Var1(1)));
    for k=1:length(known_pos)
        known_letters(n) = known_letters(n) + sum(known_pos(k)==cell2mat(words(n,1).Var1(1)));
    end
    
    %cell2mat(words(n,1).Var1(1))
end

words.lengths = lengths';
words.rem_letters = (lengths-known_letters)';
words.known_letters = known_letters';
words.solved = zeros(1,height(words))';


[success, return_letters] = find_word(data, words, letters );



% %Create a new permutation of alphabet in letters
% cont_perm = 1;
% N = length(unknown_pos);  % Length of the set.
% K = length(unknown_pos);  % Number of samples taken for each sampling.
% H = nextperm(N,K);
% 
% A = H();
% 
% new_order = zeros(1,26);
% new_order(known_pos) = known_letters;
% new_order(unknown_pos(A)) = remaining_letters;
% 
% max_matched = 0;
% while cont_perm
%     %Check whether the first word is a word and break if not
%     %     fprintf('%s\n',new_order);
%     n = 1;
%     cont = 1;
%     while cont
%         test_word = char(new_order(words{n}));
%         %     fprintf('%s\n',test_word);
%         if sum(strcmp(test_word,data{1,1}))>0
%             fprintf('Match on word %d, %s\n',n,test_word);
%             if n>max_matched
%                 max_matched = n;
%                 output_letters = char(new_order);
%                 SOLVED_WORD = char(new_order(answer_word));
% %                 fprintf('The output values are %s\n',char(new_order));
% %                 fprintf('The solved word is %s\n',char(new_order(answer_word)));
%                 if n==length(words)
%                     cont_perm = 0;
%                     break;
%                 end
%             end
%             n = n+1;
%         else
%             cont = 0;
%         end
%     end
%     if cont_perm
%         %Create the next permutation
%         A = H();
%         perm_count = perm_count + 1;
%         new_order(known_pos) = known_letters;
%         new_order(unknown_pos(A)) = remaining_letters;
%     end
%     
%     
% end

% fprintf('The output values are %s\n',char(new_order));
% fprintf('The solved word is %s\n',char(new_order(answer_word)));

