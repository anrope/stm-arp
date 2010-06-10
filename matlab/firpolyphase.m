

%% Convert FIR filter into polyphase FIR filter

clear polyphaseh;
D = 4;

for i=1:D
    k = 1;
    for j=0:((length(origh)/D)-1)
        polyphaseh(i,k) = origh(j*D+i);
        k = k+1;
    end
end


            

% static int num_coef = 51;
% 
% static int b_coefs[] = {
% 	13,	46,	101,	175,	241,
% 	271,	232,	119,	-45,	-197,
% 	-267,	-206,	-18,	222,	394,
% 	384,	146,	-254,	-639,	-780,
% 	-492,	276,	1394,	2577,	3477,
% 	3812,	3477,	2577,	1394,	276,
% 	-492,	-780,	-639,	-254,	146,
% 	384,	394,	222,	-18,	-206,
% 	-267,	-197,	-45,	119,	232,
% 	271,	241,	175,	101,	46,
% 	13 };