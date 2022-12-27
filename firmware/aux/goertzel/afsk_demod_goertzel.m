f1 = 1200;     % signal frequency 1
f2 = 2200;     % signal frequency 2
R = 1200;      % APRS Baud-rate (fixed) (not used)
st = 0.100;    % stop time

% Change these:
fs = 9615;    % sampling frequency
N = 22;       % block length (22 seems goood)

B = fs/2;     % bandwidth
df = fs/N;    % bin size (frequency resolution)
dt = 1/fs;    % time step

% dual-tone example signal
t = (0:dt:st-dt)';
x = 0.5*sin(2*pi*f1*t) + 0.5*sin(2*pi*f2*t);

%%% Goertzel
 % parameters: x: vector of samples
 %             k: desired Fourier-coefficient index
 %             N: block length
function P = goertzel(x,k,N)

  w = 2*pi*k/N;
  cw = cos(w);
  c = 2*cw;
  sw = sin(w);

  t = [0:1:N-1];
  window = 0.5 - 0.5*cos(2*pi/N*t); % N length raised-cosine windowing function
  %window = ones(N, 1); % to turn off windowing
  %window = hamming(N);
  #plot(window);

  z0 = z1 = z2 = 0;
  for n = 1 : N
    z0 = x(n) * window(n) + c*z1 - z2;
    z2 = z1;
    z1 = z0;
  end;
  I = cw*z1 - z2;
  Q = sw*z1;

  P = sqrt(I^2 + Q^2);
end

%%% Spectogram for N and fs parameter testing purposes
s = [];
for k=[1:N/2]
  s = [s goertzel(x, k, N)];
end

figure();
unit = [1:N/2] * df;    % frequency scale
plot(unit, s);
set(gca, "fontsize", 12);
grid minor on;
title(strcat("fs:", num2str(fs), "Hz, N:", num2str(N),  " BW:", num2str(B), "Hz, df:", num2str(df), "Hz"));
xlabel("Frequency [Hz]");
ylabel("Power [?]");

%%% REAL SHIT
% load sample sound
[x, fs] = audioread("aprs_9600_16bit.wav");

B = fs/2;     % bandwidth
df = fs/N;    % bin size (frequency resolution)
dt = 1/fs;    % time step

s = [];     % f1 signal presence
m = [];     % f2 signal presence
d = [];     % demodulated signal (oversampled with fs)

% in reality, this can be done real-time
for i=1:1:(N-2) * fix(length(x) / N)
  s = [s goertzel(x(i:i+N), floor(f1/df), N)];
  m = [m goertzel(x(i:i+N), floor(f2/df), N)];
  d = [d s(i)>m(i)];
end

figure();
unit = [1:(N-2) * fix(length(x) / N)]*dt;   % time scale
hold on;
plot(unit, s-m);  % s-m difference
plot(unit, d);    % demodulated signal
set(gca, "fontsize", 12);
grid minor on;
title(strcat("AFSK signal"));
xlabel("Time [s]");
ylabel("Signals");
