close all;

ds = dataset('File', fullfile("aprs_data.txt"));

df = double(ds(:,1));
%df = df - sum(df)/length(df);   %remove DC offset

%plot(df);
%hold on

df = resample(df, 8399, 44100);
%df = round((df/max(df)*500 + 512)); % imitating avr ADC
%dlmwrite('sampled_data.txt',df);
%plot(df);
%figure;

%df = df(1:9950);

t20 = [0:6]/7;
mf1200 = exp(1i*2*pi*t20*1.0);
mf2200 = exp(1i*2*pi*t20*1.83333);

plot(mf1200);
plot(mf2200);

d12 = conv(df,mf1200);
d22 = conv(df,mf2200);

ddif = min(max(abs(d12)-abs(d22),-2),2);

plot(df);
hold on

plot(ddif);
hold on

ddif = lowpass(ddif, 1200, 8399);
plot(ddif);
hold on

plot(ddif>0.5);
grid minor

dlmwrite('digital.txt',ddif>0);
eyediagram(ddif, 7, 7);
