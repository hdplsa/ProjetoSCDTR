function [] = open(folder)
    %UNTITLED4 Summary of this function goes here
    %   Detailed explanation goes here
    
    %Abre dados retirados do arduino
    time0 = csvread('time0.csv');
    ref0 = csvread('ref0.csv');
    y0 = csvread('luminous0.csv');
    duty0 = csvread('duty0.csv');
    Energy0 = csvread('Energy0.csv');
    Cerror0 = csvread('Cerror0.csv');
    Verror0 = csvread('Verror0.csv');


    time1 = csvread('time1.csv');
    ref1 = csvread('ref1.csv');
    y1 = csvread('luminous1.csv');
    duty1 = csvread('duty1.csv');
    Energy1 = csvread('Energy1.csv');
    Cerror1 = csvread('Cerror1.csv');
    Verror1 = csvread('Verror1.csv');
end

