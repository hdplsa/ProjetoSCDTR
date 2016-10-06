arduino = serial('COM3','BaudRate',9600);

fopen(arduino);

x=1:100;
y=zeros(1,length(x));

fprintf(arduino,'%i',42); 

for i = 1:length(x)
   
    tmp = fscanf(arduino,'lux = %f\n');
    if length(tmp)~=1
        continue;
    end
    y(i) = tmp;
end
   
plot(x,y)

fclose(arduino);

    
