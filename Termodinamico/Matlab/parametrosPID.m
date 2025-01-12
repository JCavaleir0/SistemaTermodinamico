sys_tf = tf(arx551);
disp('Função de Transferência do sistema (em z):');
sys_tf;
[PID_Controller, ~] = pidtune(sys_tf, 'PID');
disp('Parâmetros do controlador PID:');
disp(PID_Controller);
feedback_system = feedback(sys_tf * PID_Controller, 1);
step(feedback_system);