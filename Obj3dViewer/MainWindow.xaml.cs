using System;
using System.Windows;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Microsoft.Win32;
using System.IO;
using System.Diagnostics;

namespace Obj3dViewer
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window, INotifyPropertyChanged
	{
		const string openglViewerProgram = "D:\\ProgramProject\\My Projects\\Obj3dViewer\\x64\\Release\\OpenGLObjFileViewer.exe";
		public MainWindow()
		{
			DataContext = this;
			InitializeComponent();
			FileName = "F:\\Model\\Sample\\Monkey_sub.obj";
		}

		private string _fileName;
		public string FileName
		{
			get { return _fileName; }
			set
			{
				if (_fileName != value)
				{
					_fileName = value;
					NotifyPropertyChanged();
				}
			}
		}

		public event PropertyChangedEventHandler PropertyChanged;
		private void NotifyPropertyChanged([CallerMemberName] string propertyName = "")
		{
			PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
		}

		private void Open3dViwer(object sender, RoutedEventArgs e)
		{
			if (Path.GetExtension(FileName) != ".obj")
			{
				MessageBox.Show("请指定.obj模型路径", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
				return;
			}

			if (!File.Exists(FileName))
			{
				MessageBox.Show("文件不存在，请指定正确的文件目录与文件名", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
				return;
			}

			//MessageBox.Show(FileName);
			if (!File.Exists(openglViewerProgram))
			{
				MessageBox.Show("找不到OpenGL Viewer", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
				return;
			}

			Process.Start(openglViewerProgram, FileName);
		}

		private void SelectFilePath(object sender, RoutedEventArgs e)
		{
			OpenFileDialog dlg = new OpenFileDialog();
			dlg.Filter = "Wavefront .obj file | *.obj";
			if (dlg.ShowDialog() == true)
			{
				FileName = dlg.FileName;
			}
		}
	}
}
