clear all; close all;

global y e u t n bool_ready strings;

serial_arduino = serial('COM6','BaudRate',115200,'DataBits',8,'StopBits',1,'Parity','none');

fopen(serial_arduino);

bool_ready = 0;

N = 100; n = 1;

y = zeros(N,1);
e = zeros(N,1);
u = zeros(N,1);
t = zeros(N,1);
strings = cell(N,1);

string_type = 'y = %f;e = %f;u = %f;t = %i';

figure; h{1} = plot(t,y); title('y');
figure; h{2} = plot(t,e); title('e');
figure; h{3} = plot(t,u); title('u');

arduino_timer = timer('ExecutionMode', 'fixedRate', 'Period', 0.2, ...
    'TimerFcn', {@getData,serial_arduino,string_type, h}, 'Name', 'Pioneer_timer');
start(arduino_timer);

% pause(2);
% 
% n_ini = n-1;
% 
% pause(20);
% 
% n_last = n-1;
% 
% mmax = max(e(n_ini:n_last));
% mmin = min(e(n_ini:n_last));

for i = 1:15
    str = sprintf('2 %i\n',20*(i-1));
    fprintf(serial_arduino,str);
    
    pause(2);
    
    n_ini = n-1;
    
    pause(10);
    
    n_last = n-1;
    
    mmax(i) = max(e(n_ini:n_last));
    mmin(i) = min(e(n_ini:n_last));
    
end

% 
% 
% pause(10);
% 
% fprintf(serial_arduino,'2 150\n');
% 
% pause(10);
% 
% fprintf(serial_arduino,'2 200\n');
% 
% pause(10);
% 
% fprintf(serial_arduino,'2 250\n');
% 
% pause(10);

stop(arduino_timer);
delete(arduino_timer);

if size(instrfind) > 0
    fclose(instrfind); delete(instrfind);
end

max_error = max([mmax;-mmin]);