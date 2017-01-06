function [vec] = deleteLastZeros(vec)
    % Delete zeros at the end of the vector
    for i=1:length(vec)
        ind = length(vec)-i+1;
       if(vec(ind) ~= 0)
           break;
       end
    end
    
    vec = vec(1:ind);
end

