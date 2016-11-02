% Cria os graficos do LDR

figure;
plot(TensaoLDR(:,1), TensaoLDR(:,2));
xlabel('Tempo [s]');
ylabel('Tensão [V]');
label('Tensão no LDR a cada instante');

figure;
plot(luxLDR(:,1), luxLDR(:,2));
xlabel('Tempo [s]');
ylabel('Iluminância [lx]');
label('Iluminância medida pelo LDR');