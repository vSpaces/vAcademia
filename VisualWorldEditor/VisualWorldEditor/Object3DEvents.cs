using System;
using System.Collections.Generic;
using System.Text;

namespace VisualWorldEditor
{
    public delegate void ObjectEditedEventHandler(Object sender, ObjectEditedEventArgs args);

    public class ObjectEditedEventArgs
    {
        private Object[] editParams;

        public ObjectEditedEventArgs(Object[] _editParams)
        {
            editParams = _editParams;
        }

        public ObjectEditedEventArgs(Object _editParam)
        {
            editParams = new Object[] { _editParam };
        }

        public Object[] EditParams
        {
            get { return (editParams); }
        }
    };

    partial class Object3D
    {
        // отбработчики формы
        public event ObjectEditedEventHandler TransformationChanged;
        public event ObjectEditedEventHandler CollisionFlagChanged;
        public event ObjectEditedEventHandler NamesChanged;
        public event ObjectEditedEventHandler ScaleChanged;
        // событие изменения параметра материала объекта
        public event ObjectEditedEventHandler MaterialChanged;
        // событие изменения списка лодов с тенями
        public event ObjectEditedEventHandler ShadowsListChanged;
        // событие блокировки
        public event ObjectEditedEventHandler ModifyLevelChanged;

        protected virtual void OnCollisionFlagChanged(ObjectEditedEventArgs args)
        {
            if (CollisionFlagChanged != null)
            {
                CollisionFlagChanged(this, args);
            }
        }

        protected virtual void OnNamesChanged(ObjectEditedEventArgs args)
        {
            if (NamesChanged != null)
            {
                NamesChanged(this, args);
            }
        }

        protected virtual void OnTransformationChanged(ObjectEditedEventArgs args)
        {
            if (TransformationChanged != null)
            {
                TransformationChanged(this, args);
            }
        }

        protected virtual void OnScaleChanged(ObjectEditedEventArgs args)
        {
            if (ScaleChanged != null)
            {
                ScaleChanged(this, args);
            }
        }

        protected virtual void OnMaterialChanged(ObjectEditedEventArgs args)
        {
            if(MaterialChanged != null)
            {
                MaterialChanged(this, args);
            }
        }

        protected virtual void OnShadowsListChanged(ObjectEditedEventArgs args)
        {
            if(ShadowsListChanged != null)
            {
                ShadowsListChanged(this, args);
            }
        }

        protected virtual void OnModifyLevelChanged(ObjectEditedEventArgs args)
        {
            if(ModifyLevelChanged != null)
            {
                ModifyLevelChanged(this, args);
            }
        }
    }
}
