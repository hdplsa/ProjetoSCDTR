%% Open all data
open(' ');

%% Process vectors
time0 = deleteLastZeros(time0);
ref0 = ref0(1:length(time0));
duty0 = duty0(1:length(time0));
y0 = y0(1:length(time0));
Energy0 = Energy0(1:length(time0));
Cerror0 = Cerror0(1:length(time0));
Verror0 = Verror0(1:length(time0));

time1 = deleteLastZeros(time1);
ref1 = ref1(1:length(time1));
duty1 = duty1(1:length(time1));
y1 = y1(1:length(time1));
Energy1 = Energy1(1:length(time1));
Cerror1 = Cerror1(1:length(time1));
Verror1 = Verror1(1:length(time1));

%% Graphs from files
graphs;