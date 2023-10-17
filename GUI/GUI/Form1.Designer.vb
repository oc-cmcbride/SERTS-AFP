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
        Me.SuspendLayout()
        '
        'SerialPort1
        '
        Me.SerialPort1.PortName = "COM4"
        '
        'ComListBox
        '
        Me.ComListBox.FormattingEnabled = True
        Me.ComListBox.ItemHeight = 16
        Me.ComListBox.Location = New System.Drawing.Point(24, 23)
        Me.ComListBox.Name = "ComListBox"
        Me.ComListBox.Size = New System.Drawing.Size(120, 84)
        Me.ComListBox.TabIndex = 3
        '
        'Connect
        '
        Me.Connect.Location = New System.Drawing.Point(24, 113)
        Me.Connect.Name = "Connect"
        Me.Connect.RightToLeft = System.Windows.Forms.RightToLeft.Yes
        Me.Connect.Size = New System.Drawing.Size(75, 23)
        Me.Connect.TabIndex = 4
        Me.Connect.Text = "Connect"
        Me.Connect.UseVisualStyleBackColor = True
        '
        'Disconnect
        '
        Me.Disconnect.Location = New System.Drawing.Point(105, 113)
        Me.Disconnect.Name = "Disconnect"
        Me.Disconnect.Size = New System.Drawing.Size(94, 23)
        Me.Disconnect.TabIndex = 5
        Me.Disconnect.Text = "Disconnect"
        Me.Disconnect.UseVisualStyleBackColor = True
        '
        'ComResultTextBox
        '
        Me.ComResultTextBox.Location = New System.Drawing.Point(23, 141)
        Me.ComResultTextBox.Name = "ComResultTextBox"
        Me.ComResultTextBox.Size = New System.Drawing.Size(221, 22)
        Me.ComResultTextBox.TabIndex = 6
        Me.ComResultTextBox.Text = "ComResultTextBox"
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(8.0!, 16.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(800, 450)
        Me.Controls.Add(Me.ComResultTextBox)
        Me.Controls.Add(Me.Disconnect)
        Me.Controls.Add(Me.Connect)
        Me.Controls.Add(Me.ComListBox)
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
End Class
