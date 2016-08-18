using System;
using System.Collections;
using System.Text;

namespace VisualWorldEditor
{
    partial class HistoryStorage
    {
        private int m_capacity; // максимальная длина истории
        private int m_historyLength; // текущая длина истории
        private int m_historyCurrent; // текущее положение в истории
        private HistoryElement[] m_actionsArray; // хранилище истории

        public HistoryStorage(int capacity)
        {
            m_capacity = capacity;
            m_historyLength = 0;
            m_historyCurrent = -1;
            m_actionsArray = new HistoryElement[m_capacity];
        }

        public bool CheckDuplicateName(String newName) // проверяем есть ли в истории объект уже с таким переименованным именем
        {
             lock (this)
            {
                for (int i=0; i< m_historyLength; i++)
                {
                    if (m_actionsArray[i].cmdId == Commander.CMD_RENAME)
                    {
                        if (m_actionsArray[i].cmdParams[0] == newName)
                            return true;
                    }
                }

            }
            return false;
        }


        public bool IsAlreadyRenamed(String newName) // проверяем есть ли в истории переименованный объект из бд 
        {
            lock (this)
            {
                for (int i = 0; i < m_historyLength; i++)
                {
                    if (m_actionsArray[i].cmdId == Commander.CMD_RENAME)
                    {
                        if (m_actionsArray[i].objNames[0] == newName)
                            return true;
                    }
                }

            }
            return false;
        }

        public void AddAction(HistoryElement newAction) // добавить действие в историю
        {
            lock (this)
            {
                if (m_historyCurrent == m_capacity - 1)
                {
                    // увеличение емкости истории при переполнении
                    m_capacity += m_capacity;
                    HistoryElement[] increaseHistory = new HistoryElement[m_capacity];
                    Array.Copy(m_actionsArray, increaseHistory, m_actionsArray.Length);
                    m_actionsArray = increaseHistory;
                }

                m_actionsArray[m_historyCurrent+1] = newAction;
                m_historyCurrent++;
                m_historyLength = m_historyCurrent + 1;

                // зажигание события возможности отмены
                HistoryAllowedEventArgs undoEvargs = new HistoryAllowedEventArgs(true);
                OnUndoAllowed(undoEvargs);
                // зажигание события отсутсвия повтора
                HistoryAllowedEventArgs redoEvargs = new HistoryAllowedEventArgs(false);
                OnRedoAllowed(redoEvargs);
            }
        }

        // неиспользуемый метод
        private void ShiftHistory() // сдвинуть историю, если очредь полна
        {
            for (int actIndex = 0; actIndex < m_capacity - 1; actIndex++ )
            {
                m_actionsArray[actIndex] = m_actionsArray[actIndex + 1];
            }
        }

        public int GetHistoryPosition() // текущее положение в истории
        {
            return m_historyCurrent;
        }
        
        public int GetHistoryLength() // длина хранимой истории
        {
            return m_historyLength;
        }

        public HistoryElement GetHistoryElement(int index) // элемент истории
        {
            if(index < 0 || index > m_historyLength)
            {
                return null;
            }

            return m_actionsArray[index];
        }

        public bool isRedo() // возможен ли повтор
        {
            return (m_historyCurrent < m_historyLength - 1);         
        }

        public HistoryElement Redo() // повтор действия из истории
        {
            if(!isRedo())
            {
                return null;
            }
            m_historyCurrent++;

            HistoryAllowedEventArgs historyEvargs = null;
            // зажигание события возможности повтора
            if (isRedo())
            {
                historyEvargs = new HistoryAllowedEventArgs(true);
                OnRedoAllowed(historyEvargs);
            }
            else
            {
                historyEvargs = new HistoryAllowedEventArgs(false);
                OnRedoAllowed(historyEvargs);
            }

            // зажигание события возможности отмены
            historyEvargs = new HistoryAllowedEventArgs(true);
            OnUndoAllowed(historyEvargs); 

            return m_actionsArray[m_historyCurrent];
        }

        public bool isUndo() // возможен ли откат
        {
            return (m_historyCurrent > -1);  
        }

        public HistoryElement Undo() // откат действия
        {
            if(!isUndo())
            {
                return null;
            }
            m_historyCurrent--;

            HistoryAllowedEventArgs historyEvargs = null;
            // зажигание события возможности отмены
            if(isUndo())
            {
                historyEvargs = new HistoryAllowedEventArgs(true);
                OnUndoAllowed(historyEvargs);
            }
            else
            {
                historyEvargs = new HistoryAllowedEventArgs(false);
                OnUndoAllowed(historyEvargs);
            }

            // зажигание события возможности повтора
            historyEvargs = new HistoryAllowedEventArgs(true);
            OnRedoAllowed(historyEvargs);            

            return m_actionsArray[m_historyCurrent+1];
        }

        public void Clear() // очистка истории (например сохранили все)
        {
            m_historyCurrent = -1;
            m_historyLength = 0;

            HistoryAllowedEventArgs historyEvargs = new HistoryAllowedEventArgs(false);
            OnUndoAllowed(historyEvargs);  
            OnRedoAllowed(historyEvargs);    
        }
    }
}
