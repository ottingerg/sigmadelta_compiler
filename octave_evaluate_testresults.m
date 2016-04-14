pkg load signal;
win = ones(1,124800/32);
sigmatest_func = csvread("sigmadelta_func_testresults.csv");
periodogram(decimate(sigmatest_func(:,[2:2]),32),win,16384,3.9e6)
saveas(1,"sigmadelta_func_testresults_spectrum.pdf","pdf");
