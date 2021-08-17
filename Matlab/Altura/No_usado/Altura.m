%Transformo los datos a structs, uno por tópico
bag = rosbag('todo.bag');
pos = select(bag,'Time',[bag.StartTime+5.8 bag.EndTime-3.8],'Topic','/drone1/self_localization/pose'); 
corners = select(bag,'Time',[bag.StartTime+5.8 bag.EndTime-3.8],'Topic','Corners_ibvs');
cornersStructs = readMessages(corners,'DataFormat','struct');
posStructs = readMessages(pos,'DataFormat','struct');

%Inicializo los variables
npos=size( posStructs,  1);
ncorners=size( cornersStructs,  1);
t=0;
tiempo_total=bag.EndTime-3.8-bag.StartTime+5.8;
tiempo=linspace(0,tiempo_total,ncorners);

%Bucle para optener posición
for i = 1:npos
    z(i)=posStructs{i}.Pose.Position.Z;
end

%Bucle para optener área
for i = 1:ncorners
  if(isempty(cornersStructs{i}.Data)==0)  %comprobar si está vacío    
    t=t+1;
    %Se cogen las esquinas
    tl_x=cornersStructs{i}.Data(1);
    tl_y=cornersStructs{i}.Data(2);
    br_x=cornersStructs{i}.Data(3);
    br_y=cornersStructs{i}.Data(4);
    
    %Se calculan área
    A(t)=abs((br_x-tl_x)*(tl_y-br_y));    
  end
end

%Interpola para que area y z sean de la misma longitud
xq=1:1.52:279;
z_interp=interp1(z,xq);

%Gráficas
figure;
plot(tiempo,z_interp,'b');
title('Posición en z');
xlabel('Tiempo')
ylabel('Altura(m)')
grid on;

figure;
plot(tiempo,A,'b');
title('Area');
xlabel('Tiempo')
ylabel('Área(píxeles^2)')
grid on;

figure;
plot(A,z_interp,'b');
title('Correlación');
xlabel('Área(píxeles^2)')
ylabel('Altura(m)')
grid on;

y = smooth(z_interp);
figure;
plot(A,y,'b');
title('Correlación filtrada');
xlabel('Área(píxeles^2)')
ylabel('Altura(m)')
grid on;

poli = polyfit((A/10000),z_interp,5);
f1 = polyval(poli,(A/10000));
figure;
plot((A/10000),f1,'b');
xlabel('Área((píxeles/100)^2)')
ylabel('Altura(m)')
title('Ajuste curva polinómica');
grid on;

poli %z=0.1113*area^4-1.0642*area^3+3.8339*area^2-6.2735*area+4.119