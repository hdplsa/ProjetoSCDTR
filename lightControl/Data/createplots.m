close all;

% Cria os graficos do LDR

f = figure;
plot(TensaoLDR(:,1), TensaoLDR(:,2),'.');
xlabel('Tempo [s]');
ylabel('Tensão [V]');
title('Tensão no LDR a cada instante');
set(f,'renderer','painters')
saveas(f,'TensaoLDR','epsc');

f = figure;
plot(luxLDR(:,1), luxLDR(:,2),'.');
xlabel('Tempo [s]');
ylabel('Iluminância [lx]');
title('Iluminância medida pelo LDR');
set(f,'renderer','painters')
saveas(f,'IluminanciaLDR','epsc');