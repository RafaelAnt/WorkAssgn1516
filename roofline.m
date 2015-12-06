clear;

x = [0:0.05:32 32:1:1024];
y1 = min(13*x,256); %roofline i7
y2 = min(30*x,256); %roofline xeon

%celing de 1 core i7
x2 = (64/13):1:512;
y3 = zeros(size(x2))+64; 

%celing de 1 core XEON
x3 = (43/30):1:512;
y4 = zeros(size(x3))+43;

figure
plot(x,y1,'red',x,y2,'blue--',x2,y3,'red:',x3,y4,'blue:')
grid on
ax=gca;
ax.XLim = [0.25 512];
ax.XScale = 'log';
ax.XTick = [0.25 0.5 1 2 4 8 16 32 64 128 256 512];
ax.XTickLabel = {'0.25';'0.5';'1';'2';'4';'8';'16';'32';'64';'128';'256';'512'};

ax.YLim = [0 512];
ax.YScale = 'log';
ax.YTick = [0 1 2 4 8 16 32 64 128 256 512];

title('Roofline Model')
xlabel('Operational Intensity (FLOPs / Byte)')
ylabel('GFLOPS')
legend('i7-4720HQ','Xeon E5650','Using only 1 core','Location','northwest')


% http://www.mathworks.com/help/matlab/ref/plot.html