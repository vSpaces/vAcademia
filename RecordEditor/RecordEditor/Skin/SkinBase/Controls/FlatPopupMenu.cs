using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;
using FlashGenie; 
using System.Drawing.Imaging;

namespace FlashGenie
{
    #region FlatPopupMenu

    class FlatPopupMenu : Panel
	{
		const int BORDER = 1;
		private PopupMenuItem[] menuItems;
		private Rectangle[] itemsBounds; 
		private Hashtable subMenus  = new Hashtable();

		private TopLevelMenuItem ownerMenu;
		public TopLevelMenuItem OwnerMenu
		{
			get
			{
				return ownerMenu;
			}
			set
			{
				ownerMenu = value;
			}
		}

		private int selectedIndex = -1;
		public int SelectedIndex 
		{
			get
			{
				return selectedIndex;
			}
			set
			{
				if (selectedIndex != -1)
				{
					//hide sub menu
					if (menuItems[selectedIndex].IsParent)
					{
						PopupMenuItem parentItem = menuItems[selectedIndex];
						FlatPopupMenu subMenu = (FlatPopupMenu) subMenus[parentItem];
						if( subMenu != null)
							subMenu.Hide();
					}
					//remove selection rectangle
					this.Invalidate(itemsBounds[selectedIndex], true);						
				}
				selectedIndex = value;
				if (value != -1)
				{
					//add selection rectangle
					this.Invalidate(itemsBounds[selectedIndex], true);						
					this.Update();
				}
			}
		}

		public FlatPopupMenu()
		{
			this.Visible = false;
		}

		private void CreateSubMenu(MenuItem parentItem)
		{
			if (parentItem.MenuItems == null || parentItem.MenuItems.Count == 0)
				return;
			FlatPopupMenu childMenu = new FlatPopupMenu();
			PopupMenuItem[] items = new PopupMenuItem[parentItem.MenuItems.Count];
			foreach (MenuItem item in parentItem.MenuItems)
				items[parentItem.MenuItems.IndexOf(item)] = (PopupMenuItem) item;
			childMenu.SetItems(items);
			this.Parent.Controls.Add(childMenu);
			subMenus.Add(parentItem, childMenu);
		}

		public void SetItems(PopupMenuItem[] items)
		{
			menuItems = items;
			itemsBounds = new Rectangle[menuItems.Length];
			int width = 0;
			int height = BORDER;			
			Graphics graphics = this.CreateGraphics();
			int index = 0;
			foreach (MenuItem item in menuItems)
			{
				if (item.IsParent)
					CreateSubMenu(item);
				MeasureItemEventArgs ev = 
					new MeasureItemEventArgs(graphics, index);
				PopupMenuItem.PopupMenu_MeasureItem(item, ev);
				itemsBounds[index] = new Rectangle(BORDER, height, ev.ItemWidth, ev.ItemHeight);
				width = Math.Max(width, ev.ItemWidth);
				height += ev.ItemHeight;
				index++;
			}
			width += 10;
			for (int i = 0; i < itemsBounds.Length; i++)
				itemsBounds[i].Width = width;
			this.Size = new Size(width + BORDER*2, height + BORDER);
		}

		private void ShowSubMenu(PopupMenuItem parentItem)
		{
			FlatPopupMenu subMenu = (FlatPopupMenu) subMenus[parentItem];
			if( subMenu == null)	return;
			subMenu.Show();
			subMenu.Left = this.Left + this.Width;
			subMenu.Top = this.Top + itemsBounds[SelectedIndex].Top - 1;
			subMenu.BringToFront();
		}

		protected override void OnVisibleChanged(EventArgs e)
		{
			base.OnVisibleChanged (e);
			if (subMenus.Count == 0)
				return;
			foreach (object subMenu in subMenus.Values)
				((FlatPopupMenu) subMenu).Visible = false;
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			//Draw background
			SolidBrush brush = new SolidBrush(this.BackColor);
			e.Graphics.FillRectangle(brush, 0, 0, this.Width, this.Height);
			//Draw Items
			if (menuItems != null && menuItems.Length > 0)
			{
				int index = 0;
				foreach (MenuItem item in menuItems)
				{
					DrawItemState state = DrawItemState.None;
					if (item.Checked)
						state |= DrawItemState.Checked;
					if (selectedIndex != -1 && item == menuItems[selectedIndex])
						state |= DrawItemState.Selected;
					DrawItemEventArgs ev = 
						new DrawItemEventArgs(e.Graphics, SystemInformation.MenuFont, 
						itemsBounds[index], index, state);
					PopupMenuItem.PopupMenu_DrawItem(item, ev, true);
					
					index++;
				}
			}
			//Draw border
			Color innerColor = Color.FromArgb(238, 236, 255);
			Color cornerColor = Color.FromArgb(189, 182, 255);
			Color outerColor = Color.FromArgb(47, 37, 181);
			Point p1 = new Point(0, this.Height - 1);
			Point p2 = new Point(0, 0);
			Point p3 = new Point(this.Width - 1, 0);			
			Point p4 = new Point(this.Width - 1, this.Height - 1);
			Pen pen = new Pen(cornerColor, 1);
			e.Graphics.DrawRectangle(pen, p2.X, p2.Y, p4.X, p4.Y);
			pen.Color = innerColor;
			e.Graphics.DrawLines(pen, new Point[] {new Point(p1.X, p1.Y - 1), p2, new Point(p3.X - 1, p3.Y)});
			pen.Color = outerColor;
			e.Graphics.DrawLines(pen, new Point[] {new Point(p1.X + 1, p1.Y), p4, new Point(p3.X, p3.Y + 1)});
		}

		protected override void OnMouseHover(EventArgs e)
		{
			base.OnMouseHover(e);
			base.ResetMouseEventArgs(); 
			//Show submenu when mouse hover
			if( SelectedIndex > -1)
			{
				PopupMenuItem parentItem = menuItems[SelectedIndex];
				if (!parentItem.IsParent || !parentItem.Enabled)
					return;
				ShowSubMenu(parentItem);
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			int index = 0;
			foreach (Rectangle rect in itemsBounds)
			{
				if (rect.Contains(e.X, e.Y))
				{
					if (SelectedIndex != index && menuItems[index].Text != "-")
						SelectedIndex = index;
					break;
				}
				index++;
			}
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			base.OnMouseLeave (e);
			if (OwnerMenu != null)
			{
				Rectangle ownerRect = 
					this.OwnerMenu.Parent.RectangleToScreen(this.OwnerMenu.Bounds);
				bool subMenuShown = false;
				foreach (object menu in subMenus.Values)
					if (((FlatPopupMenu) menu).Visible)
					{
						subMenuShown = true;
						break;
					}
				if (!ownerRect.Contains(Cursor.Position) && !subMenuShown)
				{
					Thread.Sleep(200);
					this.OwnerMenu.Selected = false;
				}
			}
			else
			{
				Thread.Sleep(200);
				this.Visible = false;
			}
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			//Show submenu when mouse hover
			PopupMenuItem parentItem = menuItems[SelectedIndex];
			if (parentItem.IsParent)	
			{
				if( parentItem.Enabled)
					ShowSubMenu(parentItem);
				return;
			}
			//Process click event
			if (e.Button != MouseButtons.Left)
				return;
			int index = 0;
			foreach (Rectangle rect in itemsBounds)
			{
				if (rect.Contains(e.X, e.Y))
				{
					if (menuItems[index].Enabled && menuItems[index].Text != "-")
					{
						if (OwnerMenu != null)
							OwnerMenu.Selected = false;
						menuItems[index].ProcessClick(new EventArgs());
					}
					break;
				}
				index++;
			}
		}
	}

	#endregion
}