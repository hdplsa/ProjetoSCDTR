function getData(obj, event,arduino, string_type, h)
    
    global y e u t n bool_ready strings;
    
    try
        str = fscanf(arduino);
        strings{n} = str;
        %     if strcmp(str,'Ready\n')
        %        bool_ready = 1;
        %     end
        tmp = sscanf(str,string_type);
    catch error
        disp(error.message);
        return;
    end
    
    
    len = length(tmp);
    
    if len~=4
        return;
    end
    
    y(n) = tmp(1);
    e(n) = tmp(2);
    u(n) = tmp(3);
    t(n) = tmp(4)*1e-3;
    
    % update das figuras
    
    set(h{1}, 'XData', t(1:n), 'YData', y(1:n));
    set(h{2}, 'XData', t(1:n), 'YData', e(1:n));
    set(h{3}, 'XData', t(1:n), 'YData', u(1:n));
    
    
    n = n+1;
end