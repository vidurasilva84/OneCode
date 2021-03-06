=================================================================================
            Windows 应用程序：CSCpuUsage 概述                 
=================================================================================


//////////////////////////////////////////////////////////////////////////////////
摘要：

这个实例演示了如何使用PerfermanceCounter来得到具有以下特征的CPU使用率（CPU Usage)。

1.总的处理器时间。
2.一项特定进程的处理器时间。
3.像任务管理器一般绘制CPU使用率历史。

///////////////////////////////////////////////////////////////////////////////////
演示：

步骤1. 在Visual Studio 2010中打开这个项目。

步骤2. 生成项目，并运行 CSCpuUsage.exe。

步骤3. 检查“显示总CPU使用率”和“显示某一进程的CPU使用率”。单击复合框，选中一个进程（例如:
       taskmgr,如果任务管理器正在运行)给监视器。

	   你将看到在此应用程序上会有2张图表来显示CPU使用率历史。

///////////////////////////////////////////////////////////////////////////////////
代码逻辑：

A. 设计 CpuUsageMonitorBase 类，用来提供监视器的基本字段、事件、功能和接口。例如，
   Timer、PerformanceCounter 和 IDisposable 接口。
    
   CpuUsageMonitorBase 是一个抽象类，所以不能被实例化。当它的子类被实例化时，它们会传递
   categoryName, counterName 和 instanceName 给类 CpuUsageMonitorBase 的构造器来初始
   化performance counter。


 	this.cpuPerformanceCounter =
                new PerformanceCounter(categoryName, counterName, instanceName);


   Timer被用来得到每隔几秒钟performance counter的值，并引发CpuUsageValueArrayChanged事
   件。如果在读取performance counter数值时有任何异常发生，ErrorOccurred事件将会被激发。

B. 设计TotalCpuUsageMonitor 类以用于监视总的CPU使用率。它继承了CpuUsageMonitorBase
   类，并且定义了3个常量：
   

        const string categoryName = "Processor";
        const string counterName = "% Processor Time";
        const string instanceName = "_Total";


   为了得到总的CPU使用率，可以用上述常量来初始化performance counter.

   
      public TotalCpuUsageMonitor(int timerPeriod, int valueArrayCapacity)
            : base(timerPeriod, valueArrayCapacity, categoryName, 
               counterName, instanceName)
        { }

C. 设计 ProcessCpuUsageMonitor类来监视某一特定进程的CPU使用率。它也继承了CpuUsageMonitor-
   -Base 类，并定义了2个常量：
	
	const string categoryName = "Process";
        const string counterName = "% Processor Time";

   为了初始化一个performance counter，仍然需要实例名（进程名）。因此，该类也提供了一个方法来
   得到可用的进程。

        static PerformanceCounterCategory category;
        public static PerformanceCounterCategory Category
        {
            get
            {
                if (category == null)
                {
                    category = new PerformanceCounterCategory(categoryName);
                }
                return category;
            }
        }

        public static string[] GetAvailableProcesses()
        {          
            return Category.GetInstanceNames().OrderBy(name => name).ToArray();
        }

   
D. 设计MainForm，用于初始化totalCpuUsageMonitor 和 processCpuUsageMonitor，登记
   CpuUsageValueArrayChanged，并将CPU使用率历史显示在图表中。

        /// <summary>
        /// 激发 processCpuUsageMonitor_CpuUsageValueArrayChangedHandler来处理
        /// processCpuUsageMonitor的CpuUsageValueArrayChanged事件.
        /// </summary>
        void processCpuUsageMonitor_CpuUsageValueArrayChanged(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            this.Invoke(new EventHandler<CpuUsageValueArrayChangedEventArg>(
                processCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e);
        }

        void processCpuUsageMonitor_CpuUsageValueArrayChangedHandler(object sender,
            CpuUsageValueArrayChangedEventArg e)
        {
            var processCpuUsageSeries = 
			       chartProcessCupUsage.Series["ProcessCpuUsageSeries"];
            var values = e.Values;

            // 在图表中显示进程的CPU使用率.
            processCpuUsageSeries.Points.DataBindY(e.Values);

        }


        /// <summary>
        /// 激发totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler以处理
        /// totalCpuUsageMonitor的CpuUsageValueArrayChanged事件.
        /// </summary>
        void totalCpuUsageMonitor_CpuUsageValueArrayChanged(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            this.Invoke(new EventHandler<CpuUsageValueArrayChangedEventArg>(
                totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler), sender, e);
        }
        void totalCpuUsageMonitor_CpuUsageValueArrayChangedHandler(object sender, 
            CpuUsageValueArrayChangedEventArg e)
        {
            var totalCpuUsageSeries = chartTotalCpuUsage.Series["TotalCpuUsageSeries"];
            var values = e.Values;

            //在图表中显示总CPU使用率.
            totalCpuUsageSeries.Points.DataBindY(e.Values);

        }

      如果在读取performance counter的值时有任何异常发生，用户界面也将处理这一事件。

        /// <summary>
        /// 激发 processCpuUsageMonitor_ErrorOccurredHandler以处理processCpuUsageMonitor
        /// 的ErrorOccurred事件.
        /// </summary>
        void processCpuUsageMonitor_ErrorOccurred(object sender, ErrorEventArgs e)
        {
            this.Invoke(new EventHandler<ErrorEventArgs>(
                processCpuUsageMonitor_ErrorOccurredHandler), sender, e);
        }

        void processCpuUsageMonitor_ErrorOccurredHandler(object sender, ErrorEventArgs e)
        {
            if (processCpuUsageMonitor != null)
            {
                processCpuUsageMonitor.Dispose();
                processCpuUsageMonitor = null;
                var processCpuUsageSeries = 
				      chartProcessCupUsage.Series["ProcessCpuUsageSeries"];
                processCpuUsageSeries.Points.Clear();
            }
            MessageBox.Show(e.Error.Message);
        }       

        /// <summary>
        /// 激发 totalCpuUsageMonitor_ErrorOccurredHandler以处理totalCpuUsageMonitor的
        /// ErrorOccurred事件.
        /// </summary>
        void totalCpuUsageMonitor_ErrorOccurred(object sender, ErrorEventArgs e)
        {
            this.Invoke(new EventHandler<ErrorEventArgs>(
                totalCpuUsageMonitor_ErrorOccurredHandler),sender,e);
        }

        void totalCpuUsageMonitor_ErrorOccurredHandler(object sender, ErrorEventArgs e)
        {
            if (totalCpuUsageMonitor != null)
            {
                totalCpuUsageMonitor.Dispose();
                totalCpuUsageMonitor = null;
            }
            MessageBox.Show(e.Error.Message);
        }

///////////////////////////////////////////////////////////////////////////////////////////
参考：

PerformanceCounter Class
http://msdn.microsoft.com/en-us/library/system.diagnostics.performancecounter.aspx

Chart Class
http://msdn.microsoft.com/en-us/library/system.windows.forms.datavisualization.charting.chart.aspx

///////////////////////////////////////////////////////////////////////////////////////////////////

  


