%Abre dados retirados do arduino
time = csvread('time.csv');
ref = csvread('ref.csv');
y = csvread('luminous.csv');
duty = csvread('duty.csv');
Energy = csvread('Energy.csv');
Cerror = csvread('Cerror.csv');
Verror = csvread('Verror.csv');

%Plot dos gráficos
figure(1);
plot(time,ref);
title('Reference');
figure(2);
plot(time,y);
title('Luminosidade');
figure(3);
plot(time,duty);
title('Duty cycle');
figure(4);
plot(time,Energy);
title('Energy');
figure(5);
plot(time,Cerror);
title('Cerror');
figure(6);
plot(time,Verror);
title('Verror');
