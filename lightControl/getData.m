function y = getData(channel,baudrate,string,n)
    arduino = serial(channel,'BaudRate',baudrate);

    fopen(arduino);

    y=zeros(1,n);

    fprintf(arduino,'%i',42);

    for i = 1:n

        tmp = fscanf(arduino,string);
        if length(tmp)~=1
            continue;
        end
        y(i) = tmp;
    end
    
    fclose(arduino);
end