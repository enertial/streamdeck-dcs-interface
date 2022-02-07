import { useDrop } from "react-dnd";
import { DcsBiosDraggableItem, DcsBiosDraggableTypes } from "../api/DcsBios/DraggableItems";

import classes from "./Forms.module.css";

interface Props {
    accept: DcsBiosDraggableTypes[];
    displayText: string;
    handleDroppedItem: (item: DcsBiosDraggableItem) => void;
    onClear: () => void;
}

function DropArea({ accept, displayText, handleDroppedItem, onClear }: Props): JSX.Element {
    const [{ isOver, canDrop }, dropRef] = useDrop({
        accept: accept,
        drop: (item: DcsBiosDraggableItem) => { handleDroppedItem(item) },
        collect: monitor => ({
            isOver: monitor.isOver(),
            canDrop: monitor.canDrop(),
        })
    })

    function DropAreaBG() {
        if (isOver) {
            return classes.droppableAreaHover;
        }
        else if (canDrop) {
            return classes.droppableAreaCanDrop;
        }
        else {
            return "";
        }
    }

    function DisplayMontiorIdentifier() {
        if (displayText) {
            const bgColor = isOver ? classes.droppedItemReplace : "";
            return <div className={`${classes.droppedItem} ${bgColor}`}> {displayText}</div>;
        }
        return null;
    }

    function ClearButton() {
        if (displayText) {
            return <button className={classes.clearButton} onClick={onClear}>x</button>;
        }
        return null;
    }

    return (
        <div className={`${classes.droppableArea} ${DropAreaBG()}`} ref={dropRef}>
            <DisplayMontiorIdentifier />
            <ClearButton />
        </div>
    );

}


export default DropArea;
