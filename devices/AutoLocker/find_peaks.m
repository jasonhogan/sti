
clear B C peaks delta fingerprint deriv data ans A B C end_point freq_list;
clear i incr j k mean_val n num peak_location q sample_rate start_point;
clear tolerance true_peaks val y;

fitgood = false;
badfit = false;

% for k=1:length(dataFreq)
%     RbScanRaw1(k,1)=dataFreq(k);
%     RbScanRaw1(k,2)=dataDAQ(k);
% end

B = smooth(smooth(RbScanRaw1(:,2)));
C = diff(B);


for k=1:length(RbScanRaw1)
    data(k,1)=RbScanRaw1(k,1);
    data(k,2)=B(k,1);
end

for k=1:length(RbScanRaw1)-1
    deriv(k,1)=RbScanRaw1(k,1);
    deriv(k,2)=C(k,1);
end

start_point = min(deriv(:,1));
end_point = max(deriv(:,1));
sample_rate = (end_point-start_point)/(length(deriv)*10);
freq_list = start_point:sample_rate:end_point;

y = interp1(deriv(:,1),deriv(:,2),freq_list,'spline');
%figure;
%plot(deriv(:,1),deriv(:,2),'o',freq_list,y);

peaks=0;
incr=1;

tolerance = 0.0005;
deriv_tolerance = 0.008;

for k=1:length(y)
    start_point = max(1,k-50);
    end_point = min(k+50,length(y));
    if y(k)< tolerance
        if y(k) > -1*tolerance
            if ceil(k/10) > max(peaks)
                if max(y(start_point:end_point)) > deriv_tolerance
                    peaks(incr)=ceil(k/10);
                    incr=incr+1;
                end
            end
        end
    end
end

incr = 1;
true_peaks = 0;

for k=1:length(peaks)
    start_point=max((peaks(k)-5),1);
    end_point=min((peaks(k)+5),length(data));
    [val, num] = max(data(start_point:end_point,2));
    mean_val = mean(data(start_point:end_point,2));
    if data(peaks(k),2) > mean_val

            true_peaks(incr) = num+start_point-1;
            incr = incr+1;

    end
end
        



for k=1:length(true_peaks)
    peak_location(k,1) = data(true_peaks(k),1);
    peak_location(k,2) = data(true_peaks(k),2);
end

i=1;
for q = 2:length(peak_location)
    for j = 1:q-1
        fingerprint(i) = peak_location(q,1)-peak_location(q-j,1);
        i=i+1;
    end
end

master_fingerprint = [.060 .032 .092];

if length(fingerprint) == 3
    delta = fingerprint-master_fingerprint;
    if mean(delta) < 0.004 && std(delta) < 0.004
        fitgood = true;
    else
        warning('bad fit!!');
        badfit = true;
    end
elseif length(fingerprint) == 2
    warning('only two peaks...');
    delta_1 = [fingerprint(1) - master_fingerprint(1) fingerprint(2) - master_fingerprint(2)];
    delta_2 = [fingerprint(1) - master_fingerprint(1) fingerprint(2) - master_fingerprint(3)];
    delta_3 = [fingerprint(1) - master_fingerprint(2) fingerprint(2) - master_fingerprint(3)];
    if mean(delta_1) < 0.004 && std(delta) < 0.004
        fitgood = true;
    elseif mean(delta_2) < 0.004 && std(delta) < 0.004
        fitgood = true;
    elseif mean(delta_3) < 0.004 && std(delta) < 0.004
        fitgood = true;
    else
        warning('bad fit!!');
        badfit = true;
    end
elseif length(fingerprint) == 1
    warning('only one peak!');
else
    warning('too many peaks');
end


if fitgood == true
    figure;
    plot(data(:,1),data(:,2));
    hold on;
    for k=1:length(true_peaks)
        plot(data(true_peaks(k),1),data(true_peaks(k),2),'s', 'MarkerSize',10,'MarkerFaceColor', 'g', 'MarkerEdgeColor', 'r')
    end
end

if badfit == true
    figure;
    plot(data(:,1),data(:,2),'r');
    hold on;
    for k=1:length(true_peaks)
        plot(data(true_peaks(k),1),data(true_peaks(k),2),'s', 'MarkerSize',10,'MarkerFaceColor', 'g', 'MarkerEdgeColor', 'r')
    end
end

