close all;

fig1 = figure(1);
subplot(2,1,1);
plot(time0/1000,ref0,time0/1000,y0,time1/1000,ref1,time1/1000,y1);
title('Luminosidade');
legend('Ref 1','Ilum 1','Ref 2','Ilum 2');
xlabel('t[s]');
ylabel('Iluminância [lx]');

subplot(2,1,2);
plot(time0/1000,duty0,time1/1000,duty1);
title('Duty cycle');
legend('Sistema 2','Sistema 1');
xlabel('t[s]');

fig2 = figure(2);
subplot(3,1,1);
plot(time0/1000,Energy);
title('Energy');
xlabel('t[s]');

subplot(3,1,2);
plot(time0/1000,Cerror);
title('Cerror');
xlabel('t[s]');

subplot(3,1,3);
plot(time0/1000,Verror);
title('Verror');
xlabel('t[s]');

%% save to file
print(fig1,'lum','-depsc');
savefig(fig1,'lum');
print(fig2,'metric','-depsc');
savefig(fig2,'metric');