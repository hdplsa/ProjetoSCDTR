%% Inicializações
T = 0.2;
Kp = 0.01;
Ki = 0.4;
Kd = 0.00001;

%Simulação
sim('Simulacao.slx');

%Gráficos
plot(y(:,1),y(:,2));
title('y');
xlabel('t[s]');