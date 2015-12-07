clear;

x1 = [0:0.05:32 32:1:1024];
y1 = min(13*x1,64); %roofline i7

% ------ CPU -----------

% ILP

ilp_x = (32/13):1:512;
ilp_y = zeros(size(ilp_x))+32; 

%FMA

fma_x = (16/13):1:512;
fma_y = zeros(size(fma_x))+16; 

% vetorização (SIMD)

simd_x = (4/13):1:512;
simd_y = zeros(size(simd_x))+4;


% no floating point banlance ??

%  ----- MEMORIA --------

% Using only 1 bus
bus_x = [0.075:0.05:9.846];
bus_y = 6.5*bus_x;


% ------ RESULTADOS -----

% Ponto mmult ijk blue
ijk_x = 3.859331;
ijk_y = 0.668537;

% ponto mmult jik magenta
jik_x = 12.026937;
jik_y = 0.676101;

%ponto mmult AxB black
axb_x = 14.390132;
axb_y = 0.684633;

%ponto mmult BxA cyan
bxa_x = 18.292515;
bxa_y = 0.691783;

%ponto mmult trans green
mmult_t_x = 63.487354;
mmult_t_y = 2.297889;


figure
plot(x1,y1,'red',ilp_x,ilp_y,'magenta-.',fma_x,fma_y,'blue-.',simd_x,simd_y,'black-.',bus_x,bus_y,'green--',ijk_x,ijk_y,'blue^',jik_x,jik_y,'magenta^',mmult_t_x,mmult_t_y,'green^',bxa_x,bxa_y,'cyan^',axb_x,axb_y,'black^')
grid on
ax=gca;
ax.XLim = [0.25 512];
ax.XScale = 'log';
ax.XTick = [0.25 0.5 1 2 4 8 16 32 64 128 256 512];
ax.XTickLabel = {'0.25';'0.5';'1';'2';'4';'8';'16';'32';'64';'128';'256';'512'};

ax.YLim = [0 128];
ax.YScale = 'log';
ax.YTick = [0 1 2 4 8 16 32 64 128 256 512];

title('i7-4720HQ (1 Core)')
xlabel('Operational Intensity (FLOPs / Byte)')
ylabel('GFLOPS')
legend('Roofline','No-ILP','No-FMA','No-SIMD','1 Mem. Channel','IJK','JIK','Transpose','BxA','AxB','Location','eastoutside')


% http://www.mathworks.com/help/matlab/ref/plot.html