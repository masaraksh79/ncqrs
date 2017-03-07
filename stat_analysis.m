## perform graphing of samples to assist finding sample size

## read data from file

path='C:\Users\Matusovsky\Desktop\Masters-UC\results\'
file_name='results.txt'
conf=0.05

X = load("-ascii", char( strcat( path, file_name) ))
## Evaludate running mean
mean_X={}
for i = 1:numel(X)
  mean_X(i) = mean(X(1:i))
endfor
## Evaludate running stddev
std_X={}
for i = 1:numel(X)
  std_X(i) = std(X(1:i))
endfor
## Evaludate confidence interval (confidence mean 1.96 ~ 95%)
conf_mean={}
for i = 1:numel(X)
conf_mean(i)=1.96*cell2mat(std_X(i))/sqrt(i)
endfor

## Evaludate confidence interval (confidence mean 1.96 ~ 95%)
rel_conf={}
for i = 1:numel(X)
  rel_conf(i)=100*cell2mat(conf_mean(i))/cell2mat(mean_X(i))
endfor

## plot
figure()
plot(1:numel(X),cell2mat(rel_conf),"--+")
grid("on")
title("relative confidence interval per #samples")
figure()
plot(1:numel(X),cell2mat(mean_X),"--+")
grid("on")
title("mean per #samples")
figure()
plot(1:numel(X),cell2mat(std_X),"--+")
grid("on")
title("standard deviation per #samples")
