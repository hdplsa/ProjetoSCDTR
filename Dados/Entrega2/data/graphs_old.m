close all;

fig1 = figure(1);
subplot(2,1,1);
plot(time0/1000,ref0,time0/1000,y0);
title('Luminosidade');
legend('ref','y');
xlabel('t[s]');
subplot(2,1,2);
plot(time0/1000,duty0);
title('Duty cycle');
xlabel('t[s]');
fig2 = figure(2);
subplot(3,1,1);
plot(time0/1000,Energy0);
title('Energy');
xlabel('t[s]');
subplot(3,1,2);
plot(time0/1000,Cerror0);
title('Cerror');
xlabel('t[s]');
subplot(3,1,3);
plot(time0/1000,Verror0);
title('Verror');
xlabel('t[s]');
%% save to file
print(fig1,'lum1','-deps');
print(fig2,'metric1','-deps');

fig3 = figure(3);
subplot(2,1,1);
plot(time1/1000,ref1,time1/1000,y1);
title('Luminosidade');
xlabel('t[s]');
subplot(2,1,2);
plot(time1/1000,duty1);
title('Duty cycle');
xlabel('t[s]');
fig4 = figure(4);
subplot(3,1,1);
plot(time1/1000,Energy1);
title('Energy');
xlabel('t[s]');
subplot(3,1,2);
plot(time1/1000,Cerror1);
title('Cerror');
xlabel('t[s]');
subplot(3,1,3);
plot(time1/1000,Verror1);
title('Verror');
xlabel('t[s]');

%% save to file
print(fig3,'lum2','-deps');
print(fig4,'metric2','-deps');