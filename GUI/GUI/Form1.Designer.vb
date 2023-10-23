<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Me.SerialPort1 = New System.IO.Ports.SerialPort(Me.components)
        Me.ComListBox = New System.Windows.Forms.ListBox()
        Me.Connect = New System.Windows.Forms.Button()
        Me.Disconnect = New System.Windows.Forms.Button()
        Me.ComResultTextBox = New System.Windows.Forms.TextBox()
        Me.FileListBox = New System.Windows.Forms.ListBox()
        Me.FileName = New System.Windows.Forms.Label()
        Me.PlayButton = New System.Windows.Forms.Button()
        Me.PauseButton = New System.Windows.Forms.Button()
        Me.StopButton = New System.Windows.Forms.Button()
        Me.IndexButton = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'SerialPort1
        '
        Me.SerialPort1.PortName = "COM4"
        '
        'ComListBox
        '
        Me.ComListBox.FormattingEnabled = True
        Me.ComListBox.ItemHeight = 25
        Me.ComListBox.Location = New System.Drawing.Point(36, 36)
        Me.ComListBox.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ComListBox.Name = "ComListBox"
        Me.ComListBox.Size = New System.Drawing.Size(178, 129)
        Me.ComListBox.TabIndex = 3
        '
        'Connect
        '
        Me.Connect.Location = New System.Drawing.Point(241, 36)
        Me.Connect.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Connect.Name = "Connect"
        Me.Connect.RightToLeft = System.Windows.Forms.RightToLeft.Yes
        Me.Connect.Size = New System.Drawing.Size(112, 36)
        Me.Connect.TabIndex = 4
        Me.Connect.Text = "Connect"
        Me.Connect.UseVisualStyleBackColor = True
        '
        'Disconnect
        '
        Me.Disconnect.Location = New System.Drawing.Point(376, 36)
        Me.Disconnect.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Disconnect.Name = "Disconnect"
        Me.Disconnect.Size = New System.Drawing.Size(141, 36)
        Me.Disconnect.TabIndex = 5
        Me.Disconnect.Text = "Disconnect"
        Me.Disconnect.UseVisualStyleBackColor = True
        '
        'ComResultTextBox
        '
        Me.ComResultTextBox.Location = New System.Drawing.Point(241, 105)
        Me.ComResultTextBox.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.ComResultTextBox.Name = "ComResultTextBox"
        Me.ComResultTextBox.Size = New System.Drawing.Size(330, 31)
        Me.ComResultTextBox.TabIndex = 6
        Me.ComResultTextBox.Text = "ComResultTextBox"
        '
        'FileListBox
        '
        Me.FileListBox.FormattingEnabled = True
        Me.FileListBox.ItemHeight = 25
        Me.FileListBox.Location = New System.Drawing.Point(124, 231)
        Me.FileListBox.Name = "FileListBox"
        Me.FileListBox.ScrollAlwaysVisible = True
        Me.FileListBox.Size = New System.Drawing.Size(393, 329)
        Me.FileListBox.TabIndex = 7
        '
        'FileName
        '
        Me.FileName.AutoSize = True
        Me.FileName.Location = New System.Drawing.Point(818, 308)
        Me.FileName.Name = "FileName"
        Me.FileName.Size = New System.Drawing.Size(109, 25)
        Me.FileName.TabIndex = 8
        Me.FileName.Text = "File Name"
        '
        'PlayButton
        '
        Me.PlayButton.Location = New System.Drawing.Point(837, 427)
        Me.PlayButton.Name = "PlayButton"
        Me.PlayButton.Size = New System.Drawing.Size(75, 57)
        Me.PlayButton.TabIndex = 9
        Me.PlayButton.Text = "Play"
        Me.PlayButton.UseVisualStyleBackColor = True
        '
        'PauseButton
        '
        Me.PauseButton.Location = New System.Drawing.Point(714, 427)
        Me.PauseButton.Name = "PauseButton"
        Me.PauseButton.Size = New System.Drawing.Size(98, 57)
        Me.PauseButton.TabIndex = 10
        Me.PauseButton.Text = "Pause"
        Me.PauseButton.UseVisualStyleBackColor = True
        '
        'StopButton
        '
        Me.StopButton.Location = New System.Drawing.Point(939, 427)
        Me.StopButton.Name = "StopButton"
        Me.StopButton.Size = New System.Drawing.Size(98, 57)
        Me.StopButton.TabIndex = 11
        Me.StopButton.Text = "Stop"
        Me.StopButton.UseVisualStyleBackColor = True
        '
        'IndexButton
        '
        Me.IndexButton.Location = New System.Drawing.Point(252, 599)
        Me.IndexButton.Name = "IndexButton"
        Me.IndexButton.Size = New System.Drawing.Size(146, 48)
        Me.IndexButton.TabIndex = 12
        Me.IndexButton.Text = "Index"
        Me.IndexButton.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(12.0!, 25.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(1200, 703)
        Me.Controls.Add(Me.IndexButton)
        Me.Controls.Add(Me.StopButton)
        Me.Controls.Add(Me.PauseButton)
        Me.Controls.Add(Me.PlayButton)
        Me.Controls.Add(Me.FileName)
        Me.Controls.Add(Me.FileListBox)
        Me.Controls.Add(Me.ComResultTextBox)
        Me.Controls.Add(Me.Disconnect)
        Me.Controls.Add(Me.Connect)
        Me.Controls.Add(Me.ComListBox)
        Me.Margin = New System.Windows.Forms.Padding(4, 5, 4, 5)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents SerialPort1 As IO.Ports.SerialPort
    Friend WithEvents ComListBox As ListBox
    Friend WithEvents Connect As Button
    Friend WithEvents Disconnect As Button
    Friend WithEvents ComResultTextBox As TextBox
    Friend WithEvents FileListBox As ListBox
    Friend WithEvents FileName As Label
    Friend WithEvents PlayButton As Button
    Friend WithEvents PauseButton As Button
    Friend WithEvents StopButton As Button
    Friend WithEvents IndexButton As Button
End Class
