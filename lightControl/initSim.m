%% Inicializações
T = 0.2;

Kp = 0.002;
Ki = 0.03;
Kd = 0.00000;

%Kp = 0.01;
%Ki = 0.4;
%Kd = 0.00001;

K = 68.92;
teta = -24.05;

%K = 30.26;
%teta = -13.37;

%Simulação
sim('Simulacao.slx');

%Gráficos
plot(y(:,1),y(:,2));
title('y');
xlabel('t[s]');