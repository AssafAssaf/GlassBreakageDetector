function [J, grad] = costFunctionReg(theta, X, y, lambda)
% Compute cost and gradient for logistic regression with regularization

    m = length(y); % number of training examples

    J = 0;
    grad = zeros(size(theta));

    xi = X * theta;
    J = sum(-y'*log(sigmoid(xi)) - (1 - y')*log(1 - sigmoid(xi)))/m + lambda*sum(theta(2:end).^2) / (2*m);

    temp = theta.*[0; ones(length(theta)-1,1)];
    grad = (sigmoid(xi) - y)'*X/m + lambda*temp'/m;


end
