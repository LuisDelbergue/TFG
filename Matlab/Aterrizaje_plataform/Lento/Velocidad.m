%PASOS PREVIOS LINUX
%cd ~/bagfiles 
%rosbag record -O velocidad /drone1/motion_reference/speed 
%rosbag info 'todo.bag'
%Escribir Velocidad para ejecutar en Matlab

%Transforman el .bag a un struct
bag = rosbag('todo.bag');
velocidad = select(bag,'Time',[bag.StartTime bag.EndTime-6.87],'Topic','/drone1/motion_reference/speed');
msgStructs = readMessages(velocidad,'DataFormat','struct');

%Se inicializa las variables
n=size( msgStructs,  1); %Numero mensajes
tiempo_total=bag.EndTime-bag.StartTime-6.87;
Vx=zeros(n,1);     
Vy=zeros(n,1); 
Vz=zeros(n,1);

for i = 1:n
    %Se cogen las coordenadas
    Vx(i)=msgStructs{i}.Twist.Linear.X;
    Vy(i)=msgStructs{i}.Twist.Linear.Y;
    Vz(i)=msgStructs{i}.Twist.Linear.Z;    
end


%Gráficas
t=linspace(0,tiempo_total,n);

figure;
subplot(1,3,1);
plot(t,Vx,'r','LineWidth',1.5);
ylabel('Velocidad(m/s)');
xlabel('Tiempo(s)');
title('Eje X');
grid on;

subplot(1,3,2);
plot(t,Vy,'b','LineWidth',1.5);
ylabel('Velocidad(m/s)');
xlabel('Tiempo(s)');
title('Eje Y');
grid on;

subplot(1,3,3);
plot(t,Vz,'k','LineWidth',1.5);
ylabel('Velocidad(m/s)');
xlabel('Tiempo(s)');
title('Eje Z');
grid on;