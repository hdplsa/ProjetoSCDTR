% Cria os graficos do LDR

figure;
plot(TensaoLDR(:,1), TensaoLDR(:,2));
xlabel('Tempo [s]');
ylabel('Tens�o [V]');
label('Tens�o no LDR a cada instante');

figure;
plot(luxLDR(:,1), luxLDR(:,2));
xlabel('Tempo [s]');
ylabel('Ilumin�ncia [lx]');
label('Ilumin�ncia medida pelo LDR');