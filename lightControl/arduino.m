clear all; close all;

if size(instrfind) > 0
    fclose(instrfind); delete(instrfind);
end

global y e u t n;
serial_arduino = serial('/dev/cu.wchusbserial1d10','BaudRate',115200,'DataBits',8,'StopBits',1,'Parity','none');

fopen(serial_arduino);

N = 10000; n = 1;

y = zeros(N,1);
e = zeros(N,1);
u = zeros(N,1);
t = zeros(N,1);

string_type = 'y = %f;e = %f;u = %f;t = %i\n';

figure; h{1} = plot(t,y);
figure; h{2} = plot(t,e);
figure; h{3} = plot(t,u);

arduino_timer = timer('ExecutionMode', 'fixedRate', 'Period', 0.2, ...
    'TimerFcn', {@getData,serial_arduino,string_type, h}, 'Name', 'Pioneer_timer');
start(arduino_timer);

pause(10);

stop(arduino_timer);
delete(arduino_timer);