%% Open all data
open_files;

%% Process vectors
time0 = deleteLastZeros(time0)-time0(1);
ref0 = ref0(1:length(time0));
duty0 = duty0(1:length(time0));
y0 = y0(1:length(time0));
Energy0 = Energy0(1:length(time0));
Cerror0 = Cerror0(1:length(time0));
Verror0 = Verror0(1:length(time0));

time1 = deleteLastZeros(time1)-time1(1);
ref1 = ref1(1:length(time1));
duty1 = duty1(1:length(time1));
y1 = y1(1:length(time1));
Energy1 = Energy1(1:length(time1));
Cerror1 = Cerror1(1:length(time1));
Verror1 = Verror1(1:length(time1));

Energy = Energy0(1:length(time0));
Cerror = Cerror0(1:length(time0));
Verror = Verror0(1:length(time0));

%% Calculo de m?tricas
time0 = time0/1000;
time1 = time1/1000;

%Energia
Energy = zeros(1,length(time0));
for n=2:length(time0)
    for k = 2:n
        Energy(n) = Energy(n) + (duty0(k-1)/255)*(time0(k)-time0(k-1))+(duty1(k-1)/255)*(time1(k)-time1(k-1));
    end
end

%Cerror
Cerror = zeros(1,length(time0));
Cerror(1) = (1/length(time0))*(max(ref0(1)-y0(1),0)+max(ref1(1)-y1(1),0)); 
for n=2:length(time0)
   Cerror(n) = Cerror(n-1)+(1/length(time0))*(max(ref0(n)-y0(n),0)+max(ref1(n)-y1(n),0)); 
end


%Vflicker
Vflicker = zeros(1,length(time0));
for n=10:length(time0)
    Vflicker(n) = (1/(n*0.02^2))*(abs(y0(n)-2*y0(n-1)+y0(n-2))+abs(y1(n)-2*y1(n-1)+y1(n-2)));
    if(sum(ref0(n-9:n)) ~= 50*10 && sum(ref0(n-9:n)) ~= 10*10)
       Vflicker(n)=0; 
    end
    if(sum(ref1(n-9:n)) ~= 50*10 && sum(ref1(n-9:n)) ~= 10*10)
       Vflicker(n)=0; 
    end
end

%% Graphs from files
graphs;