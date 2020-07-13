﻿using System;

namespace TogglDesktop
{
public class SingleInstanceManager<T> : Microsoft.VisualBasic.ApplicationServices.WindowsFormsApplicationBase
    where T : System.Windows.Application, new()
{
    T app;
    public SingleInstanceManager()
    {
        base.IsSingleInstance = true;
    }

    public event Action BeforeStartup;

    protected override bool OnStartup(Microsoft.VisualBasic.ApplicationServices.StartupEventArgs e)
    {
        BeforeStartup?.Invoke();
        app = new T();
        app.Run();
        return false;
    }

    protected override void OnStartupNextInstance(Microsoft.VisualBasic.ApplicationServices.StartupNextInstanceEventArgs e)
    {
        app.MainWindow?.ShowOnTop();
        if (e.CommandLine.Count > 0 && e.CommandLine[0].StartsWith(Program.StartupUri))
        {
            (app.MainWindow as MainWindow)?.ProcessStartupUri(e.CommandLine[0]);
        }
    }
}
}