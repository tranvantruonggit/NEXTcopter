#!/usr/bin/octave
y=[150224, 150224];
ticks=['NON-OPT'; 'OPT'; ];
fh=barh(y); ylabel('Optimization'); xlabel('Code Size [B]'); title('The effect of the optimization on the binary size'); set(gca, 'ytickLabel', ticks); xt = get(gca, 'xtick'); set(gca, 'xticklabel', sprintf('%1.1E|', xt)); grid on; axis auto; saveas(fh, 'results.png', 'png');
saveas(fh, 'results.png', 'png');