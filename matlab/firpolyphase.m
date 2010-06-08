

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

            
