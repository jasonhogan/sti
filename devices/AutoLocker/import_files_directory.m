
files = dir('\\atomsrv1\EP\Data\RbScannerAutoSave\*.csv');
for i = 1:length(files)
    files(i).name
    RbScanRaw1 = importdata(strcat('\\atomsrv1\EP\Data\RbScannerAutoSave\', files(i).name));
    try
        find_peaks;
    catch
         disp '** Error running find_peaks'
    end
end