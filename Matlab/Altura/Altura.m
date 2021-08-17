%Transformo los datos a structs, uno por tópico
bag = rosbag('todo.bag');
pos = select(bag,'Time',[bag.StartTime+2 bag.EndTime-2.1],'Topic','/drone1/self_localization/pose'); 
corners = select(bag,'Time',[bag.StartTime+2 bag.EndTime-2.1],'Topic','Corners_ibvs');
cornersStructs = readMessages(corners,'DataFormat','struct');
posStructs = readMessages(pos,'DataFormat','struct');

%Inicializo los variables
npos=size( posStructs,  1);
ncorners=size( cornersStructs,  1);
t=0;
tiempo_total=bag.EndTime-2.1-bag.StartTime+2;
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
    A(t)=(tl_x-br_x)*(tl_y-br_y);    
  end
end

%Interpola para que area y z sean de la misma longitud
xq=1:1.49:177;
z_interp=interp1(z,xq);

%Gráficas
figure;
subplot(1,4,1);
plot(tiempo,z_interp,'b','LineWidth',1.5);
title('Posición en z');
xlabel('Tiempo')
ylabel('Altura(m)')
grid on;

subplot(1,4,2);
plot(tiempo,A,'b','LineWidth',1.5);
title('Area');
xlabel('Tiempo')
ylabel('Área(píxeles^2)')
grid on;

subplot(1,4,3);
plot(A,z_interp,'b','LineWidth',1.5);
title('Correlación');
xlabel('Área(píxeles^2)')
ylabel('Altura(m)')
grid on;

%Ajuste para obtener una aproximación polinómica
poli = polyfit((A/10000),z_interp,3); %A/10000 para que esté mejor condicionada
f1 = polyval(poli,(A/10000));
subplot(1,4,4);
plot((A/10000),f1,'b','LineWidth',1.5);
legend('z=-13.4*A^3+22.8*A^2-13.4*A+4.1');
xlabel('Área((píxeles/100)^2)');
ylabel('Altura(m)');
title('Ajuste curva polinómica');
grid on;

poli 