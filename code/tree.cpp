void tree::Init(arena *Arena)
{
    ClearArena(Arena);
    this->Arena = Arena;
    First = 0; 
    Size = 0;
}

bool tree::IsEmpty()
{
    return (First == 0);
}

i32 tree::AddChildByID(i32 Value, char *Name, i32 ID)
{
    i32 ChildID = Size;
    if(First == 0)
    {
        First = PushStruct(Arena, node);
        First->Value = Value;
        First->Name = Name;
        First->ID = ID;
        First->Parent = 0;
        First->NextSibling = 0;
        First->BackSibling = 0;
    }
    else
    {
        node *Actual = FindNodeByID(ID);
        Actual->AddChild(Value, Name, ChildID, Arena);
    }
    ++Size;
    return ChildID;
}
void tree::node::AddChild(i32 Value, char *Name, i32 ID, arena *Arena)
{
    if(Child == 0)
    {
        Child = PushStruct(Arena, node);
        Child->ID = ID;
        Child->Name = Name;
        Child->Value = Value;
        Child->Parent = this;
    }
    else
    {
        // TODO: Get the last sibling and add one more...
        node *Node = Child;
        while(Node->NextSibling)
        {
            Node = Node->NextSibling;
        }
        Node->NextSibling = PushStruct(Arena, node);
        Node->NextSibling->ID = ID;
        Node->NextSibling->Name = Name;
        Node->NextSibling->Value = Value;
        Node->NextSibling->Parent = this;
        Node->NextSibling->BackSibling = Node;
        Node->NextSibling->NextSibling = 0;
    }
}

tree::node *tree::FindNodeByID(i32 ID)
{
    node *Node = First;
    while(Node->Child)
    {
        if(Node->ID == ID)
        {
            return Node;
        }
        Node = Node->Child;
    }
    while(Node)
    {        
        if(Node->ID == ID)
        {
            return Node;
        } 

        if(Node->Child)
        {
            Node = Node->Child;
        }
        else
        {
            if(!Node->Child && !Node->NextSibling)
            {
                Node = Node->Parent;
                while(!Node->NextSibling)
                {
                    Node = Node->Parent;
                } 
            }
            Node = Node->NextSibling;
        }
    }
    return 0;
}
