y=[0, 28010, 56380, 32120, 12230, 18630, 20720, 38930, 47980, 59230, 46270, 39190, 41970, 45450, 28860, 24650, 46530, 62480, 65040, 73720, 39090, 72670, 41480, 66470, 30160, 39330, 31890, 25730, 22030, 32760, 25530, 63100, 16150, 57880, 48060, 77900, 56050, 76060, 42180, 40780, 18020, 52090, 25290, 52200, 44040, 53320, 51750, 53490, 64200, 81210, 26720, 48890, 29630, 55350, 24150, 65570, 41660, 86410, 62560, 75140, 58520, 45920];
erosszam=[1:length(y)];

y2=[22750, 5700, 1870, 8440, 0, 26150, 7740, 22710, 20890, 30280, 17100, 28870, 32030, 36590, 9010, 24630, 12320, 44030, 50950, 57150, 47950, 46090, 41320, 71650, 9180, 38380, 52430];
atlagszam=[1:length(y2)];
figure;

plot(atlagszam,y2,"r", "linewidth", 2, "marker",".",erosszam,y,"b", "linewidth", 2, "marker","." );
axis([0 70 0 100000]);
