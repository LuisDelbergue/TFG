%PASOS PREVIOS LINUX
%cd ~/bagfiles (se guardan en capeta_personal\rosbagfiles)
%rosbag record -O posicion /drone1/self_localization/pose (obtiene archivo posicion.bag, historial del tópico)
%rosbag info 'todo.bag'
%Escribir Posicion para ejecutar en Matlab

%Transforman el .bag a un struct
bag = rosbag('todo.bag');
pos = select(bag,'Time',[bag.StartTime bag.EndTime-3.05],'Topic','/drone1/self_localization/pose'); 
msgStructs = readMessages(pos,'DataFormat','struct');

%Se inicializa las variables
n=size( msgStructs,  1); %Numero mensajes
tiempo_total=bag.EndTime-bag.StartTime-3.05;
suma_x=0;  
suma_y=0;  
xi=2*(ones(n,1));   %x ideal
yi=0*ones(n,1);      %y ideal
zi=0.2*(ones(n,1)); %z ideal
x=zeros(n,1);     
y=zeros(n,1); 
z=zeros(n,1);
RMSE_x=zeros(n,1);
RMSE_y=zeros(n,1);

for i = 1:n
    
    %Se cogen las coordenadas
    x(i)=msgStructs{i}.Pose.Position.X;
    y(i)=msgStructs{i}.Pose.Position.Y;
    z(i)=msgStructs{i}.Pose.Position.Z;

    %Se hace la suma
    suma_x=(xi(i)-x(i))^2+suma_x;
    suma_y=(yi(i)-y(i))^2+suma_y;
    
    %RMSE
    RMSE_x(i)=sqrt(suma_x/i);
    RMSE_y(i)=sqrt(suma_y/i);
    
end

%Gráficas
t=linspace(0,tiempo_total,n);

figure;
subplot(2,3,3);
hold on;
plot(t,z,'k','LineWidth',1.5);
plot(t,zi,'g','LineWidth',0.8);
legend('Posición real','Posición ideal');
ylabel('Posición(m)');
xlabel('Tiempo(s)');
title('Eje Z');
grid on;

subplot(2,3,6);
axis([-3 3 -3 3])
P = [xi(1),y(i),0.2] ;   % you center point 
L = [0.6,0.6,0.2] ;  % your cube dimensions 
O = P-L/2 ;       % Get the origin of cube so that P is at center 
plotcube(L,O,.8,[1 0 0]);   % use function plotcube 
hold on;
plot3(x,y,z,'m','LineWidth',1.5);
title('Trayectoria 3D');
xlabel('x');
ylabel('y');
zlabel('z');
grid on;

subplot(2,3,1);
plot(t,RMSE_x,'r','LineWidth',1.5);
title('Eje X');
ylabel('RMSE(m)');
xlabel('Tiempo(s)');
grid on;

subplot(2,3,4);
plot(t,RMSE_y,'b','LineWidth',1.5);
title('Eje Y');
ylabel('RMSE(m)');
xlabel('Tiempo(s)');
grid on;

subplot(2,3,2);
hold on;
plot(t,x,'r','LineWidth',1.5);
plot(t,xi,'g','LineWidth',0.8);
legend('Posición real','Posición ideal');
ylabel('Posición(m)');
xlabel('Tiempo(s)');
title('Eje X');
grid on;

subplot(2,3,5);
hold on;
plot(t,y,'b','LineWidth',1.5);
plot(t,yi,'g','LineWidth',0.8);
legend('Posición real','Posición ideal');
ylabel('Posición(m)');
xlabel('Tiempo(s)');
title('Eje Y');
grid on;