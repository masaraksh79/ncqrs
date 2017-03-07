a1 = cell2mat({1,1,0,1,0,0,0,1,0,0,0,0})
a2 = cell2mat({1,1,1,0,1,0,0,0,1,0,0,0})
a3 = cell2mat({1,0,1,1,0,1,0,0,0,1,0,0})

for i=1:12
  y(1,i)=(a1(1,i)+a2(1,i)+a3(1,i))/3
endfor
for i=1:12
  if (i<2)
     z(1,i)=y(1,i)
  else
     z(1,i)=0.1*(y(1,i))+0.9*(z(1,i-1))
  endif
endfor

x=1:1:12

figure(1)
subplot(2,1,1)
plot(x,y)
grid("on")
xlabel("superframes")
title("System immediate violation")
subplot(2,1,2)
plot(x,z)
grid("on")
xlabel("superframes")
title("System immediate violation after MA (factor of 0.1)")
