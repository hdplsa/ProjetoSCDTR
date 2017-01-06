close all;

fig1 = figure(1);
subplot(2,1,1);
plot(time0,ref0,time0,y0,time1,ref1,time1,y1);
title('Luminosidade');
legend('Ref 1','Ilum 1','Ref 2','Ilum 2');
xlabel('t[s]');
ylabel('Iluminancia [lx]');
xlim([0 20]);

subplot(2,1,2);
plot(time0,duty0,time1,duty1);
title('Duty cycle');
legend('Sistema 1','Sistema 2');
xlabel('t[s]');
xlim([0 20]);

fig2 = figure(2);
subplot(3,1,1);
plot(time0,Energy);
title('Energy');
xlabel('t[s]');
ylabel('E[J]');
xlim([0 20]);

subplot(3,1,2);
plot(time0,Cerror);
title('Cerror');
xlabel('t[s]');
ylabel('Iluminancia[lx]');
xlim([0 20]);

subplot(3,1,3);
plot(time0,Vflicker);
title('Vflicker');
xlabel('t[s]');
ylabel('[lx/s^2]');
xlim([0 20]);

%% save to file
print(fig1,'lum','-depsc');
savefig(fig1,'lum');
print(fig2,'metric','-depsc');
savefig(fig2,'metric');