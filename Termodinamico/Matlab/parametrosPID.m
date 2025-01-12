% Nome do modelo Simulink
modelName = 'PID_Control_Simulink';

% Criar um novo modelo Simulink
new_system(modelName);
open_system(modelName);

% Adicionar os blocos
add_block('simulink/Sources/Step', [modelName '/Step Input'], 'Position', [100, 150, 130, 180]);
add_block('simulink/Commonly Used Blocks/Scope', [modelName '/Scope'], 'Position', [700, 150, 730, 180]);
add_block('simulink/Continuous/PID Controller', [modelName '/PID Controller'], 'Position', [250, 150, 300, 180]);
add_block('simulink/Continuous/Transfer Fcn', [modelName '/Transfer Function'], 'Position', [450, 150, 500, 180]);
add_block('simulink/Math Operations/Sum', [modelName '/Sum'], 'Position', [600, 120, 630, 200], 'Inputs', '|+-');

% Conectar os blocos
add_line(modelName, 'Step Input/1', 'PID Controller/1');
add_line(modelName, 'PID Controller/1', 'Transfer Function/1');
add_line(modelName, 'Transfer Function/1', 'Sum/1');
add_line(modelName, 'Sum/1', 'Scope/1');
add_line(modelName, 'Transfer Function/1', 'Sum/2', 'autorouting', 'on');

% Configurar os parâmetros do controlador PID e a função de transferência
load_system_model = 'arx551'; % Certifique-se de que o modelo ARX está no workspace
sys_tf = tf(arx551); % Converter para função de transferência
[PID_Controller, ~] = pidtune(sys_tf, 'PID');

% Obter parâmetros do controlador PID
Kp = PID_Controller.Kp;
Ki = PID_Controller.Ki;
Kd = PID_Controller.Kd;

% Definir parâmetros nos blocos
set_param([modelName '/Transfer Function'], 'Numerator', mat2str(sys_tf.Numerator{1}), ...
    'Denominator', mat2str(sys_tf.Denominator{1}));
set_param([modelName '/PID Controller'], 'P', num2str(Kp), 'I', num2str(Ki), 'D', num2str(Kd));

% Salvar e executar o modelo
save_system(modelName);
sim(modelName);
