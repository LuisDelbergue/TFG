%PASOS PREVIOS LINUX
%cd ~/bagfiles 
%rosbag record -O corners /Corners_ibvs 
%rosbag info 'todo.bag'
%Escribir Corners para ejecutar en Matlab

%Transforman el .bag a un struct
bag = rosbag('todo.bag');
corners = select(bag,'Time',[bag.StartTime+2 bag.EndTime-3.55],'Topic','Corners_ibvs');
msgStructs = readMessages(corners,'DataFormat','struct');

%Se inicializa las variables
n=size( msgStructs,  1); %Numero mensajes 
tiempo_total=bag.EndTime-bag.StartTime-5.55;
t=0;
suma_x=0;  
suma_y=0; 

for i = 1:n
  if(isempty(msgStructs{i}.Data)==0)  %comprobar si está vacío 
    t=t+1;
  end
end

Ex=zeros(t,1);
Ey=zeros(t,1); 
tl_x=zeros(t,1);
tl_y=zeros(t,1);
br_x=zeros(t,1);
br_y=zeros(t,1);
t=0;
RMSE_x=zeros(n,1);
RMSE_y=zeros(n,1);

for i = 1:n
  if(isempty(msgStructs{i}.Data)==0)  %comprobar si está vacío
    
    t=t+1;
    %Se cogen las esquinas
    tl_x(t)=msgStructs{i}.Data(1);
    tl_y(t)=msgStructs{i}.Data(2);
    br_x(t)=msgStructs{i}.Data(3);
    br_y(t)=msgStructs{i}.Data(4);
    
    %Se calculan los centros
    Cx=(br_x(t)-tl_x(t))/2+tl_x(t);
    Cy=(tl_y(t)-br_y(t))/2+br_y(t);
    
    %Se calculan los errores (no son los mismos ejes)
    Ey(t)=320-Cx;
    Ex(t)=240-Cy;  
    
    %Se hace la suma
    suma_x=(Ex(t))^2+suma_x;
    suma_y=(Ey(t))^2+suma_y;
    
    %RMSE
    RMSE_x(t)=sqrt(suma_x/t);
    RMSE_y(t)=sqrt(suma_y/t);
  end
end

%Gráficas
p=linspace(0,tiempo_total,t);
s=zeros(t,1);

figure;
subplot(1,3,1);
hold on;
plot(p,Ex,'r','LineWidth',1.5);
plot(p,Ey,'b','LineWidth',1.5);
plot(p,s,'g','LineWidth',0.8);
legend('Error en x','Error en y','Error ideal');
title('Errores(píxels)');
xlabel('Tiempo(s)');
grid on;

subplot(1,3,2);
hold on;
plot(p,RMSE_x,'r','LineWidth',1.5);
plot(p,RMSE_y,'b','LineWidth',1.5);
title('Root-mean-square error');
legend('RMSE en x','RMSE en y');
ylabel('RMSE(píxels)');
xlabel('Tiempo(s)');
grid on;

%Filtrar
n=size( br_x,  1);

subplot(1,3,3);
hold on;
axis([-10 650 -10 490])
plot(tl_x(1),tl_y(1),'ko','LineWidth',1.5);
plot(br_x(n),br_y(n),'kx','LineWidth',1.5);
plot(br_x,br_y,'m','LineWidth',0.9);
plot(br_x,tl_y,'m','LineWidth',0.9);
plot(tl_x,br_y,'m','LineWidth',0.9);
plot(tl_x,tl_y,'m','LineWidth',0.9);
plot(br_x(1),br_y(1),'ko','LineWidth',1.5);
plot(br_x(1),tl_y(1),'ko','LineWidth',1.5);
plot(tl_x(1),br_y(1),'ko','LineWidth',1.5);
plot(br_x(n),tl_y(n),'kx','LineWidth',1.5);
plot(tl_x(n),br_y(n),'kx','LineWidth',1.5);
plot(tl_x(n),tl_y(n),'kx','LineWidth',1.5);
title('Imagen de la trayectoria esquinas de la H');
legend('Posición incial','Posición final');
xlabel('Píxeles en x');
ylabel('Píxeles en y');
grid on;
