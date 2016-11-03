close all;

% Cria os graficos do LDR

f = figure;
plot(TensaoLDR(:,1), TensaoLDR(:,2),'.');
xlabel('Tempo [s]');
ylabel('Tens�o [V]');
title('Tens�o no LDR a cada instante');
set(f,'renderer','painters')
saveas(f,'TensaoLDR','epsc');

f = figure;
plot(luxLDR(:,1), luxLDR(:,2),'.');
xlabel('Tempo [s]');
ylabel('Ilumin�ncia [lx]');
title('Ilumin�ncia medida pelo LDR');
set(f,'renderer','painters')
saveas(f,'IluminanciaLDR','epsc');